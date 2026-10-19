<script>
	const tickExample = `client frame (about 60 per second)                 server tick (exactly 60 per second)
PrepUpdate       GetInput: keyboard -> KeyboardInput
                 TryConnect: CONNECT until accepted
StateTransition  SendInput: INPUT ───────UDP───────> network thread: InputQueue of the player
                                                    game thread:
                                                      timeouts, new players
                                                      pop one input per player -> Velocity, bullet
                                                      FixedUpdate: Movement, Script, Collider2D
                                                      flushDestroyQueue
                 ReadSocket: rebuild <────UDP─────── SNAPSHOT of the tick
FixedUpdate      (no entity to simulate)
PostUpdate       RenderSystem -> IRenderer -> raylib`;

	const layersExample = `layer                 folder                              knows about
game (R-Type)         src/game-rtype, src/server          everything below
plugins               src/plugins/{Renderer,Physics,      the engine core, and one library each
                      Network}                            (raylib, nothing, Asio)
shared gameplay       src/components, src/systems,        the engine core
                      src/common
engine core           src/engine                          only the C++ standard library`;
</script>

<svelte:head>
	<title>Architecture - R-Type docs</title>
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

{#snippet a(href, text)}<a {href} class="text-[#ff8a3d] hover:underline">{text}</a>{/snippet}

<h1 class="text-3xl font-semibold">Architecture</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	The layers and subsystems of the engine, the client and the server
</p>

<p class="max-w-[100ch] leading-7">
	R-Type is made of two programs built from the same engine. The <strong>server</strong>
	({@render c('r-type_server')}) runs the whole game and is the only authority. The
	<strong>client</strong> ({@render c('r-type_client')}) sends the keys of the player and draws what
	the server sends back. Both are built on a small ECS engine (entities, components and systems)
	that knows nothing about R-Type, raylib or the network.
</p>

<h2 class="mt-9 text-lg font-semibold">Subsystem view</h2>
<figure class="mt-4 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#0a0e0c] p-4">
	<svg
		viewBox="0 0 960 520"
		class="h-auto w-full min-w-[720px] font-mono"
		role="img"
		aria-labelledby="arch-title arch-desc"
	>
		<title id="arch-title">R-Type architecture</title>
		<desc id="arch-desc">
			The client and the server each stack, from top to bottom: the R-Type game code, the
			plugins or server subsystems, the shared gameplay components and systems, the engine core,
			and the third-party libraries. They talk only through UDP packets: INPUT from the client,
			SNAPSHOT from the server.
		</desc>
		<defs>
			<marker id="arrow" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse">
				<path d="M 0 0 L 10 5 L 0 10 z" fill="#ff8a3d" />
			</marker>
		</defs>

		<!-- client -->
		<rect x="10" y="10" width="400" height="500" rx="10" fill="none" stroke="#1e2b26" stroke-width="2" />
		<text x="30" y="40" fill="#ff8a3d" font-size="16" font-weight="700">CLIENT  r-type_client</text>

		<rect x="30" y="60" width="360" height="60" rx="6" fill="#121a17" stroke="#3fbed6" />
		<text x="45" y="85" fill="#d8e4de" font-size="14" font-weight="600">Game: Application</text>
		<text x="45" y="105" fill="#7f9189" font-size="12">game loop, fixed step, adds the plugins</text>

		<rect x="30" y="135" width="112" height="120" rx="6" fill="#121a17" stroke="#3fbed6" />
		<text x="40" y="158" fill="#d8e4de" font-size="13" font-weight="600">Renderer</text>
		<text x="40" y="180" fill="#7f9189" font-size="11">IRenderer</text>
		<text x="40" y="196" fill="#7f9189" font-size="11">RayLibRenderer</text>
		<text x="40" y="212" fill="#7f9189" font-size="11">RenderSystem</text>
		<text x="40" y="228" fill="#7f9189" font-size="11">GetInput</text>
		<text x="40" y="244" fill="#7f9189" font-size="11">Sprite</text>

		<rect x="154" y="135" width="112" height="120" rx="6" fill="#121a17" stroke="#3fbed6" />
		<text x="164" y="158" fill="#d8e4de" font-size="13" font-weight="600">Physics</text>
		<text x="164" y="180" fill="#7f9189" font-size="11">Velocity</text>
		<text x="164" y="196" fill="#7f9189" font-size="11">Collider2D</text>
		<text x="164" y="212" fill="#7f9189" font-size="11">MovementSystem</text>
		<text x="164" y="228" fill="#7f9189" font-size="11">Collider2DSys.</text>

		<rect x="278" y="135" width="112" height="120" rx="6" fill="#121a17" stroke="#3fbed6" />
		<text x="288" y="158" fill="#d8e4de" font-size="13" font-weight="600">Network</text>
		<text x="288" y="180" fill="#7f9189" font-size="11">Socket</text>
		<text x="288" y="196" fill="#7f9189" font-size="11">TryConnect</text>
		<text x="288" y="212" fill="#7f9189" font-size="11">SendInput</text>
		<text x="288" y="228" fill="#7f9189" font-size="11">ReadSocket</text>

		<rect x="30" y="270" width="360" height="60" rx="6" fill="#121a17" stroke="#1e2b26" />
		<text x="45" y="295" fill="#d8e4de" font-size="14" font-weight="600">Shared gameplay</text>
		<text x="45" y="315" fill="#7f9189" font-size="12">Position, Type, Script, ScriptSystem, protocol</text>

		<rect x="30" y="345" width="360" height="80" rx="6" fill="#121a17" stroke="#ff8a3d" />
		<text x="45" y="370" fill="#d8e4de" font-size="14" font-weight="600">Engine core: Scene</text>
		<text x="45" y="390" fill="#7f9189" font-size="12">EntityManager, ComponentManager,</text>
		<text x="45" y="406" fill="#7f9189" font-size="12">SystemManager, ResourceManager, Prefabs</text>

		<rect x="30" y="440" width="175" height="50" rx="6" fill="none" stroke="#1e2b26" stroke-dasharray="4 3" />
		<text x="45" y="470" fill="#7f9189" font-size="13">raylib 6.0</text>
		<rect x="215" y="440" width="175" height="50" rx="6" fill="none" stroke="#1e2b26" stroke-dasharray="4 3" />
		<text x="230" y="470" fill="#7f9189" font-size="13">Asio 1.30.2</text>

		<!-- server -->
		<rect x="550" y="10" width="400" height="500" rx="10" fill="none" stroke="#1e2b26" stroke-width="2" />
		<text x="570" y="40" fill="#ff8a3d" font-size="16" font-weight="700">SERVER  r-type_server</text>

		<rect x="570" y="60" width="360" height="60" rx="6" fill="#121a17" stroke="#3fbed6" />
		<text x="585" y="85" fill="#d8e4de" font-size="14" font-weight="600">Game logic: GameLoop + Game</text>
		<text x="585" y="105" fill="#7f9189" font-size="12">game thread, 60 ticks/s, prefabs, waves</text>

		<rect x="570" y="135" width="174" height="120" rx="6" fill="#121a17" stroke="#3fbed6" />
		<text x="580" y="158" fill="#d8e4de" font-size="13" font-weight="600">Network thread</text>
		<text x="580" y="180" fill="#7f9189" font-size="11">Network::receiveLoop</text>
		<text x="580" y="196" fill="#7f9189" font-size="11">players, timeouts</text>
		<text x="580" y="212" fill="#7f9189" font-size="11">InputQueue (per player)</text>
		<text x="580" y="228" fill="#7f9189" font-size="11">sendSnapshot</text>

		<rect x="756" y="135" width="174" height="120" rx="6" fill="#121a17" stroke="#3fbed6" />
		<text x="766" y="158" fill="#d8e4de" font-size="13" font-weight="600">Physics systems</text>
		<text x="766" y="180" fill="#7f9189" font-size="11">MovementSystem</text>
		<text x="766" y="196" fill="#7f9189" font-size="11">Collider2DSystem</text>
		<text x="766" y="212" fill="#7f9189" font-size="11">Sprite (data only)</text>

		<rect x="570" y="270" width="360" height="60" rx="6" fill="#121a17" stroke="#1e2b26" />
		<text x="585" y="295" fill="#d8e4de" font-size="14" font-weight="600">Shared gameplay</text>
		<text x="585" y="315" fill="#7f9189" font-size="12">Position, Type, PlayerId, Script, protocol</text>

		<rect x="570" y="345" width="360" height="80" rx="6" fill="#121a17" stroke="#ff8a3d" />
		<text x="585" y="370" fill="#d8e4de" font-size="14" font-weight="600">Engine core: Scene</text>
		<text x="585" y="390" fill="#7f9189" font-size="12">same code as the client,</text>
		<text x="585" y="406" fill="#7f9189" font-size="12">TickTimeManager for the fixed tick</text>

		<rect x="570" y="440" width="360" height="50" rx="6" fill="none" stroke="#1e2b26" stroke-dasharray="4 3" />
		<text x="585" y="470" fill="#7f9189" font-size="13">Asio 1.30.2 (raylib headers only)</text>

		<!-- network link -->
		<line x1="392" y1="175" x2="566" y2="175" stroke="#ff8a3d" stroke-width="2" marker-end="url(#arrow)" />
		<text x="420" y="165" fill="#ff8a3d" font-size="12">INPUT, CONNECT</text>
		<line x1="566" y1="215" x2="392" y2="215" stroke="#ff8a3d" stroke-width="2" marker-end="url(#arrow)" />
		<text x="420" y="235" fill="#ff8a3d" font-size="12">SNAPSHOT, ACCEPT</text>
		<text x="452" y="200" fill="#d8e4de" font-size="13" font-weight="700">UDP</text>
	</svg>
	<figcaption class="mt-3 text-[13.5px] text-[#7f9189]">
		Each block only uses the blocks below it. Orange boxes are the engine core, dashed boxes are
		third-party libraries downloaded by CPM.
	</figcaption>
</figure>

<h2 class="mt-9 text-lg font-semibold">The layers</h2>
{@render code(layersExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		<strong>Engine core</strong>: the {@render c('Scene')} stores entities, components, systems and
		resources. See {@render a('/developer/ecs', 'How the ECS works')}.
	</li>
	<li>
		<strong>Rendering</strong>: the Renderer plugin. Systems only draw through the
		{@render c('IRenderer')} interface, raylib stays inside {@render c('RayLibRenderer')} and
		{@render c('GetInput')}. See {@render a('/developer/game-loop', 'How the game loop works')}.
	</li>
	<li>
		<strong>Networking</strong>: the Network plugin on the client, the {@render c('Network')} class and
		its thread on the server. Both only exchange the packets of
		{@render c('src/common/rtype_protocol.hpp')}. See
		{@render a('/developer/udp-server', 'How the server works')} and
		{@render a('/developer/network-client', 'How the client network works')}.
	</li>
	<li>
		<strong>Game logic</strong>: {@render c('GameLoop')} and {@render c('Game')} on the server. They
		create and update entities through the Scene only, and never touch a socket or a window.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">One frame, one tick</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	How the subsystems run one after the other. Each side runs on its own clock: the client draws as
	fast as its screen allows (60 FPS target), the server always runs 60 ticks per second.
</p>
{@render code(tickExample)}

<h2 class="mt-9 text-lg font-semibold">How the subsystems stay apart</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Systems never call each other. They share data through components (on entities) and resources
		(one value per scene, such as {@render c('KeyboardInput')} or {@render c('Socket')}).
	</li>
	<li>
		A plugin ({@render c('IPlugin')}) registers its own components, resources and systems in
		{@render c('build')}. The client {@render c('Application')} only lists the plugins it wants.
	</li>
	<li>
		On the server, the network thread never touches the Scene: it only fills the input queues, see
		{@render a('/developer/network-threads', 'How the server threads work')}.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Known couplings</h2>
<p class="mt-3 max-w-[100ch] leading-7">Places where the layers are not fully apart yet:</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		The server uses the Renderer {@render c('Sprite')} component to describe how entities look, so it
		includes the raylib headers (but does not link raylib).
	</li>
	<li>
		{@render c('GameLoop::buildSnapshot')} picks the enemy variant by comparing the texture path with
		{@render c('Assets/Bydos2.png')}.
	</li>
	<li>
		{@render c('GetInput')} calls raylib directly, so changing the graphics library also means
		rewriting it.
	</li>
	<li>
		{@render c('Application')} and {@render c('IPlugin')} live in {@render c('src/game-rtype')}
		instead of the engine folder.
	</li>
</ul>
