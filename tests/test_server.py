#!/usr/bin/env python3
"""Minimal UDP server speaking rtype_protocol, to test the client by hand.

Usage: ./tests/test_server.py [--port 4242] [--rate 20] [--no-autostart] [--reject REASON]

Packet layouts follow src/common/rtype_protocol.hpp (big-endian integers).
SNAPSHOT entity layout is not fixed by the protocol yet; this server sends
[x (u32), y (u32), type (u8)] per entity, see ENTITY_FORMAT.
"""

import argparse
import socket
import struct
import time

PROTOCOL_VERSION = 1
MAX_PLAYERS = 4

CONNECT = 0x10
DISCONNECT = 0x11
GAME_START = 0x20
INPUT = 0x30
CONNECT_ACCEPT = 0x90
CONNECT_REJECT = 0x91
PLAYER_LEFT = 0x92
GAME_STARTED = 0xA0
SNAPSHOT = 0xC0

UP, DOWN, LEFT, RIGHT, CHARGE = 1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4
INPUT_UNUSED_BITS = 0xE0

SERVER_FULL = 0x01
WRONG_VERSION = 0x02
GAME_ALREADY_STARTED = 0x03
NOT_CONNECTED = 0x04
REJECT_REASONS = {
    "full": SERVER_FULL,
    "version": WRONG_VERSION,
    "started": GAME_ALREADY_STARTED,
}

PLAYER, ENEMY, BULLET, OBSTACLE = 0, 1, 2, 3

ENTITY_FORMAT = ">IIB"
SPEED = 5
WIDTH, HEIGHT = 800, 600


class Player:
    def __init__(self, player_id):
        self.id = player_id
        self.x = 100
        self.y = 100 + player_id * 100
        self.last_input_tick = 0
        self.last_ack_tick = 0


class TestServer:
    def __init__(self, port, rate, autostart, reject):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind(("0.0.0.0", port))
        self.sock.settimeout(1.0 / rate)
        self.period = 1.0 / rate
        self.autostart = autostart
        self.reject = reject
        self.players = {}  # addr -> Player
        self.started = False
        self.tick = 0
        print(f"listening on udp/{port}, {rate} snapshots/s")

    def send(self, addr, data):
        self.sock.sendto(data, addr)

    def broadcast(self, data):
        for addr in self.players:
            self.send(addr, data)

    def free_id(self):
        used = {p.id for p in self.players.values()}
        return next(i for i in range(MAX_PLAYERS) if i not in used)

    def on_connect(self, addr, data):
        if addr in self.players:
            self.send(addr, bytes([CONNECT_ACCEPT, self.players[addr].id]))
            return
        reason = self.reject
        if reason is None:
            if len(data) < 2 or data[1] != PROTOCOL_VERSION:
                reason = WRONG_VERSION
            elif len(self.players) >= MAX_PLAYERS:
                reason = SERVER_FULL
            elif self.started:
                reason = GAME_ALREADY_STARTED
        if reason is not None:
            print(f"{addr} rejected (reason {reason:#04x})")
            self.send(addr, bytes([CONNECT_REJECT, reason]))
            return
        player = Player(self.free_id())
        self.players[addr] = player
        print(f"{addr} connected as player {player.id}")
        self.send(addr, bytes([CONNECT_ACCEPT, player.id]))
        if self.autostart:
            self.start_game()

    def start_game(self):
        if self.started:
            return
        self.started = True
        print(f"game started at tick {self.tick}")
        self.broadcast(bytes([GAME_STARTED]) + struct.pack(">I", self.tick))

    def on_input(self, addr, data):
        player = self.players.get(addr)
        if player is None:
            self.send(addr, bytes([CONNECT_REJECT, NOT_CONNECTED]))
            return
        if len(data) < 10:
            print(f"{addr} short INPUT ({len(data)} bytes)")
            return
        tick, bits, ack = struct.unpack(">IBI", data[1:10])
        if bits & INPUT_UNUSED_BITS:
            print(f"{addr} INPUT with unused bits set: {bits:#04x}")
        if tick < player.last_input_tick:
            return
        player.last_input_tick = tick
        player.last_ack_tick = ack
        if bits & UP:
            player.y -= SPEED
        if bits & DOWN:
            player.y += SPEED
        if bits & LEFT:
            player.x -= SPEED
        if bits & RIGHT:
            player.x += SPEED
        player.x = max(0, min(WIDTH, player.x))
        player.y = max(0, min(HEIGHT, player.y))

    def on_disconnect(self, addr):
        player = self.players.pop(addr, None)
        if player is None:
            return
        print(f"{addr} (player {player.id}) left")
        self.broadcast(bytes([PLAYER_LEFT, player.id]))
        if not self.players:
            self.started = False

    def handle(self, data, addr):
        if not data:
            return
        kind = data[0]
        if kind == CONNECT:
            self.on_connect(addr, data)
        elif kind == DISCONNECT:
            self.on_disconnect(addr)
        elif kind == GAME_START:
            self.start_game()
        elif kind == INPUT:
            self.on_input(addr, data)
        else:
            print(f"{addr} unknown packet {kind:#04x} ({len(data)} bytes)")

    def snapshot(self):
        entities = [(p.x, p.y, PLAYER) for p in self.players.values()]
        # a static obstacle and an enemy moving back and forth to check rendering
        entities.append((400, 300, OBSTACLE))
        entities.append((WIDTH - 100, (self.tick * 2) % HEIGHT, ENEMY))
        payload = b"".join(struct.pack(ENTITY_FORMAT, x, y, t) for x, y, t in entities)
        return struct.pack(">BIH", SNAPSHOT, self.tick, len(entities)) + payload

    def run(self):
        next_tick = time.monotonic()
        while True:
            try:
                data, addr = self.sock.recvfrom(1024)
                self.handle(data, addr)
            except socket.timeout:
                pass
            except ConnectionResetError:
                pass
            now = time.monotonic()
            if now < next_tick:
                continue
            next_tick += self.period
            self.tick += 1
            if self.started:
                self.broadcast(self.snapshot())


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--port", type=int, default=4242)
    parser.add_argument("--rate", type=int, default=20, help="snapshots per second")
    parser.add_argument("--no-autostart", action="store_true",
                        help="wait for a GAME_START packet instead of starting on first connect")
    parser.add_argument("--reject", choices=REJECT_REASONS,
                        help="reject every CONNECT with this reason")
    args = parser.parse_args()
    reject = REJECT_REASONS[args.reject] if args.reject else None
    server = TestServer(args.port, args.rate, not args.no_autostart, reject)
    try:
        server.run()
    except KeyboardInterrupt:
        print()


if __name__ == "__main__":
    main()
