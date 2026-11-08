<script>
	const code0 = `# A minimal R-Type client written only from the protocol documentation, to check that it is enough.
# Usage: python3 proto_client.py <port>
import socket
import struct
import sys
import time

PORT = int(sys.argv[1])
SERVER = ('127.0.0.1', PORT)
PROTOCOL_ID = 0x4752
DATA, CONNECT_REQUEST, CONNECT_ACCEPTED, CONNECT_REJECTED, KEEP_ALIVE, DISCONNECT = range(6)
ENTITY_STATE, DESPAWN, INPUT_STATE, PEER_LEFT, SCORE_UPDATE = 0xFF01, 0xFF02, 0xFF03, 0xFF04, 0xFF05
TRANSFORM_CODEC = 0xAD82ABBB

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.settimeout(0.01)
seq = 0


def send_packet(kind, body=b''):
    global seq
    sock.sendto(struct.pack('<HBH', PROTOCOL_ID, kind, seq) + body, SERVER)
    seq = (seq + 1) & 0xFFFF


def send_input(tick, bits):
    payload = struct.pack('<II', tick, bits)
    send_packet(DATA, struct.pack('<HH', INPUT_STATE, len(payload)) + payload)


def fnv1a(name):
    h = 2166136261
    for byte in name.encode():
        h = ((h ^ byte) * 16777619) & 0xFFFFFFFF
    return h


assert fnv1a('Transform') == TRANSFORM_CODEC, hex(fnv1a('Transform'))

# 1. Handshake
my_peer = None
for attempt in range(25):
    send_packet(CONNECT_REQUEST, struct.pack('<H', 2))
    try:
        data, _ = sock.recvfrom(2048)
    except socket.timeout:
        time.sleep(0.2)
        continue
    pid, kind, _ = struct.unpack_from('<HBH', data)
    if pid == PROTOCOL_ID and kind == CONNECT_ACCEPTED:
        my_peer = struct.unpack_from('<I', data, 5)[0]
        break
    if kind == CONNECT_REJECTED:
        sys.exit(f'rejected, reason {data[5]}')
print('connected, my peer id', my_peer)

# 2. Play: START as host for 1 s, then hold RIGHT; decode everything
entities = {}
score = 0
seen_prefabs = set()
my_ship_x = []
start = time.time()
tick = 0
while time.time() - start < 6:
    bits = (1 << 4) if time.time() - start < 1 else (1 << 3)
    send_input(tick, bits)
    tick += 1
    deadline = time.time() + 1 / 60
    while time.time() < deadline:
        try:
            data, _ = sock.recvfrom(2048)
        except socket.timeout:
            continue
        pid, kind, _ = struct.unpack_from('<HBH', data)
        if pid != PROTOCOL_ID or kind != DATA:
            continue
        offset = 5
        while offset < len(data):
            mtype, length = struct.unpack_from('<HH', data, offset)
            payload = data[offset + 4:offset + 4 + length]
            offset += 4 + length
            if mtype == ENTITY_STATE:
                _, net_id, owner, name_len = struct.unpack_from('<IIIH', payload)
                prefab = payload[14:14 + name_len].decode()
                p = 14 + name_len
                count = payload[p]
                p += 1
                transform = None
                for _ in range(count):
                    codec, size = struct.unpack_from('<IH', payload, p)
                    block = payload[p + 6:p + 6 + size]
                    p += 6 + size
                    if codec == TRANSFORM_CODEC:
                        assert size == 36, size
                        transform = struct.unpack('<9f', block)
                assert p == len(payload), 'trailing bytes'
                entities[net_id] = (prefab, owner, transform)
                seen_prefabs.add(prefab)
                if owner == my_peer and transform:
                    my_ship_x.append(transform[0])
            elif mtype == DESPAWN:
                _, n = struct.unpack_from('<IH', payload)
                for net_id in struct.unpack_from(f'<{n}I', payload, 6):
                    entities.pop(net_id, None)
            elif mtype == SCORE_UPDATE:
                score = struct.unpack('<Ii', payload)[1]

send_packet(DISCONNECT)
print('prefabs seen:', sorted(seen_prefabs))
print('my ship x: first', round(my_ship_x[0]), 'last', round(my_ship_x[-1]))
print('entities alive now:', len(entities), 'score:', score)`;
</script>

<svelte:head>
	<title>Network protocol - R-Type docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Network protocol</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">What R-Type sends over the Gringo protocol, and how to write a new client</p>

<p class="mt-3 max-w-[100ch] leading-7">
	R-Type uses the network protocol of the Gringo Engine as it is: the bytes, packets, messages and timings are specified in <a href="/gringo/developer/network-protocol" class="text-accent hover:underline">How the protocol works</a>. This page adds what is specific to R-Type, so that a new client can be written from these two pages only. The client at the end of this page was written from them and tested against <code>r-type_server</code>.
</p>

<h2 class="mt-9 text-lg font-semibold">Connecting</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The server listens on UDP <code>127.0.0.1:4242</code> by default (<code>./r-type_server [ip] port</code>).</li>
	<li>Send <code>ConnectRequest</code> with version <code>2</code>. Keep the <code>peerId</code> of the <code>ConnectAccepted</code>: it identifies your ship.</li>
	<li>At most 4 players. During a match the server rejects new players with <code>Full</code>; they can join once everybody is back in the lobby.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Input bits</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	Send an <code>InputState</code> every tick (60 per second) with the actions held:
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Bit</th><th class="px-4 py-3">Value</th><th class="px-4 py-3">Action</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">0</td><td class="px-4 py-3 align-top"><code>0x01</code></td><td class="px-4 py-3 align-top">Move up</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">1</td><td class="px-4 py-3 align-top"><code>0x02</code></td><td class="px-4 py-3 align-top">Move down</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">2</td><td class="px-4 py-3 align-top"><code>0x04</code></td><td class="px-4 py-3 align-top">Move left</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">3</td><td class="px-4 py-3 align-top"><code>0x08</code></td><td class="px-4 py-3 align-top">Move right</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">4</td><td class="px-4 py-3 align-top"><code>0x10</code></td><td class="px-4 py-3 align-top">Start the match (only the host's counts: the player with the lowest peer id)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">5</td><td class="px-4 py-3 align-top"><code>0x20</code></td><td class="px-4 py-3 align-top">Charge: held, it charges; released, it fires the charged shot</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	The ship fires on its own: there is no fire bit. The server moves the ship at 400 pixels per second and keeps it on screen.
</p>

<h2 class="mt-9 text-lg font-semibold">Coordinates</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The world is <strong>1920 by 1080</strong>, x to the right, y <strong>down</strong>, origin at the top left.</li>
	<li>A <code>Transform</code> gives the <strong>center</strong> of the entity. R-Type only uses x and y; z, the scales and the rotations stay at their defaults.</li>
	<li>Entities can be slightly outside the screen while they enter or leave.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Prefabs the server sends</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	The <code>prefab</code> name of an <code>EntityState</code> tells what the entity is. The images are in <code>R-Type/assets/</code>; the animated ones have a <code>.anim</code> file describing their frames.
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Prefab</th><th class="px-4 py-3">What it is</th><th class="px-4 py-3">Image</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>PlayerBlue</code>, <code>PlayerGreen</code>, <code>PlayerMagenta</code>, <code>PlayerRed</code></td><td class="px-4 py-3 align-top">The ships of slots 1 to 4. <code>owner</code> is the player's peer id</td><td class="px-4 py-3 align-top"><code>Player&lt;Color&gt;.png</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Enemy</code>, <code>Enemy2</code></td><td class="px-4 py-3 align-top">The two kinds of Bydos</td><td class="px-4 py-3 align-top"><code>Bydos1Anim</code>, <code>Bydos2Anim</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>PowArmor</code></td><td class="px-4 py-3 align-top">The power-up carrier</td><td class="px-4 py-3 align-top"><code>PowArmor</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>BossStage1</code> to <code>BossStage9</code></td><td class="px-4 py-3 align-top">The boss; the server replaces it with the next stage as it takes damage</td><td class="px-4 py-3 align-top"><code>BossStage1</code> to <code>9</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>PlayerBullet</code>, <code>PlayerBulletUp</code>, <code>PlayerBulletDown</code></td><td class="px-4 py-3 align-top">Ship shots, straight and diagonal</td><td class="px-4 py-3 align-top"><code>PlayerShot</code>, <code>PlayerShotUp</code>, <code>PlayerShotDown</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>BulletCharge1</code> to <code>BulletCharge5</code></td><td class="px-4 py-3 align-top">Charged shots, by level</td><td class="px-4 py-3 align-top"><code>BulletCharge1</code> to <code>5</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>EnemyBullet</code>, <code>BossBullet</code></td><td class="px-4 py-3 align-top">Enemy and boss shots</td><td class="px-4 py-3 align-top"><code>EnemyBullet</code>, <code>BossBullet</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>PowerUp</code></td><td class="px-4 py-3 align-top">The power-up to pick up</td><td class="px-4 py-3 align-top"><code>PowerUp</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Explosion</code></td><td class="px-4 py-3 align-top">A destroyed enemy, for half a second</td><td class="px-4 py-3 align-top"><code>Explosion</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>StageMusic</code></td><td class="px-4 py-3 align-top">No image: it exists while a match runs (play <code>stage1.mp3</code>)</td><td class="px-4 py-3 align-top"></td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	The stars of the background are not sent: each client draws its own.
</p>

<h2 class="mt-9 text-lg font-semibold">Match state and score</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><strong>Lobby or match</strong>: a match runs while a <code>StageMusic</code> entity exists. When it is despawned, everybody is back in the lobby.</li>
	<li><strong>Players</strong>: the ship entities present are the connected players. A player is in the slot of its prefab (Blue 1, Green 2, Magenta 3, Red 4).</li>
	<li><strong>A destroyed ship</strong> is not despawned: the server moves it far out of the screen until the next match.</li>
	<li><strong>Score</strong>: the <code>ScoreUpdate</code> message carries the team score.</li>
	<li>R-Type sends no game message (no type below <code>0xFF00</code>).</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">A minimal client</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	This Python client follows the two pages and nothing else: it connects, starts the match as the host, holds right, and decodes every entity. Run against <code>./r-type_server 24555</code>, it prints the prefabs it saw (<code>Enemy</code>, <code>PlayerBlue</code>, <code>StageMusic</code>...) and its ship moving from x 100 to the right edge of the screen.
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code0}</code></pre>
