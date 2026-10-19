<script>
	const flowExample = `client                                   server
GetInput      keys pressed -> KeyboardInput
SendInput     [INPUT, tick, keys, ...] ─────> network thread: InputQueue of the player
                                              game loop, once per tick: pop one input
                                                executeInput() -> Velocity, maybe a bullet
                                                MovementSystem -> Position`;

	const keysExample = `Z or Up      -> UP       0x01
S or Down    -> DOWN     0x02
Q or Left    -> LEFT     0x04
D or Right   -> RIGHT    0x08
Space        -> CHARGE   0x10   (not used by the server yet)`;

	const executeExample = `velocity = (0, 0)
UP     -> dy -= 400        DOWN  -> dy += 400       (PLAYER_SPEED)
LEFT   -> dx -= 400        RIGHT -> dx += 400
cooldown over -> PlayerBullet at (x + 50, y), speed 550 px/s, cooldown = 0.2 s`;
</script>

<svelte:head>
	<title>How inputs work - R-Type docs</title>
</svelte:head>

{#snippet code(text)}
	<pre
		class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code
			>{text}</code
		></pre>
{/snippet}

{#snippet c(text)}<code
		class="rounded border border-[#1e2b26] bg-[#121a17] px-1.5 py-0.5 font-mono text-[13.5px]"
		>{text}</code
	>{/snippet}

<h1 class="text-3xl font-semibold">How inputs work</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How a key pressed on the client moves the player on the server
</p>

<p class="max-w-[100ch] leading-7">
	The client never moves its player itself. It sends the keys that are pressed, and the server
	applies them in the game. The player then moves on screen when the next snapshot arrives.
</p>
{@render code(flowExample)}

<h2 class="mt-9 text-lg font-semibold">On the client</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('GetInput')} fills the {@render c('KeyboardInput')} resource at the start of each
	frame. Then {@render c('SendInput')} turns the keys into bits and sends one
	{@render c('INPUT')} per frame, even when no key is pressed, as long as the client is connected.
	The tick of the packet goes up by one each time.
</p>
{@render code(keysExample)}

<h2 class="mt-9 text-lg font-semibold">The input queue</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The network thread pushes the keys of each 10 bytes {@render c('INPUT')} into the
	{@render c('InputQueue')} of the player. The queue has its own mutex and keeps only the 2 newest
	inputs ({@render c('MAX_STORED_INPUTS')}): when a client sends faster than the server ticks, the
	oldest inputs are dropped instead of piling up and adding lag.
</p>

<h2 class="mt-9 text-lg font-semibold">On the server</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	At each tick, the game loop pops one input per player and calls
	{@render c('executeInput')}. It sets the {@render c('Velocity')} of the player entity, and fires
	whenever the cooldown of that player is over, whatever the keys: {@render c('CHARGE')} is not read,
	so the ships fire on their own. Then the
	{@render c('MovementSystem')} moves every entity, and a script keeps the player inside the
	1920 x 1080 screen.
</p>
{@render code(executeExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		When no input arrives during a tick, the velocity does not change: the player keeps moving
		until the next input. The client sends one every frame, so it stops on the next frame.
	</li>
	<li>Pressing two opposite keys (UP and DOWN, LEFT and RIGHT) cancels the movement.</li>
	<li>
		Each player has its own fire cooldown in {@render c('_shootCooldowns')}, so two players can
		shoot at the same time.
	</li>
	<li>An {@render c('INPUT')} that is not exactly 10 bytes is ignored.</li>
	<li>
		The tick of an {@render c('INPUT')} is the client's own frame counter, it has nothing to do with
		the server tick.
	</li>
</ul>
