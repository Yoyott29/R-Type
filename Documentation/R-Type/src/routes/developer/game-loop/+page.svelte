<script>
	const mainExample = `int main(int argc, char **argv)
{
    std::string ip = rtype_protocol::DEFAULT_ADDRESS;
    uint16_t port = rtype_protocol::DEFAULT_PORT;

    if (checkArgs(argc, argv, ip, port) == 84)
        return 84;
    try {
        Application application(ip, port);
        application.run();
    } catch (std::exception const &error) {
        std::cerr << "Fatal error: " << error.what() << "\\n";
        return 84;
    }
    return 0;
}`;

	const constructorExample = `Application::Application(const std::string &serverAddress, uint16_t serverPort)
{
    // setup basic components and resources
    _scene.registerComponent<Position>();
    _scene.registerComponent<Script>();
    _scene.registerComponent<Type>();
    _scene.addResource<AppAlive>();

    // setup plugins
    addPlugin(std::make_shared<RayLibRenderer>());
    addPlugin(std::make_shared<Physics>());
    addPlugin(std::make_shared<Client>(serverAddress, serverPort));

    // setup gameplay systems, after the Physics systems in the fixed update
    _scene.registerSystem<ScriptSystem>(FixedUpdate);
    ...
}`;

	const loopExample = `// src/engine/core/Time.hpp
constexpr float FIXED_DELTA_TIME = ...; // 1/60 s
// src/game-rtype/Application.cpp
#define MAX_FRAME_TIME 0.25f

void Application::run()
{
    using Clock = std::chrono::steady_clock;
    Clock::time_point previous = Clock::now();
    float accumulator = 0.0f;

    while (_scene.getResource<AppAlive>().alive) {
        Clock::time_point now = Clock::now();
        float frameTime =
            std::min(std::chrono::duration<float>(now - previous).count(), MAX_FRAME_TIME);
        previous = now;
        accumulator += frameTime;

        _scene.runSchedule(PrepUpdate);
        _scene.runSchedule(StateTransition);
        while (accumulator >= FIXED_DELTA_TIME) {
            _scene.runSchedule(FixedUpdate);
            _scene.flushDestroyQueue();
            accumulator -= FIXED_DELTA_TIME;
        }
        _scene.runSchedule(Update);
        _scene.runSchedule(PostUpdate);
    }

    for (auto &plugin : _plugins)
        plugin->cleanup(*this);
}`;

	const stepsExample = `frame takes 1/60 s   -> 1 FixedUpdate,  1 render
frame takes 1/30 s   -> 2 FixedUpdates, 1 render
frame takes 1 s      -> clamped to 0.25 s, about 15 FixedUpdates, 1 render`;

	const schedulesExample = `PrepUpdate       GetInput (keyboard, window close), TryConnect
StateTransition  SendInput, ReadSocket (apply the server snapshot)
FixedUpdate      MovementSystem, Collider2DSystem, ScriptSystem  (0..n times)
Update           nothing registered on the client yet
PostUpdate       RenderSystem (draws the frame)`;

	const rendererExample = `class IRenderer
{
  public:
    virtual ~IRenderer() = default;

    virtual void beginFrame() = 0;
    virtual void clearBackground() = 0;
    virtual void drawSprite(Position const &position, Sprite const &sprite) = 0;
    virtual void endFrame() = 0;
};`;

	const errorsExample = `std::exception
├── FileNotFound                  a texture file cannot be loaded (RayLibRenderer)
├── UnknownComponentType          unknown entity type, or player id above 3, in a snapshot (ReadSocket)
├── MultipleComponentDefinition   component registered twice, or added twice to an entity
└── std::runtime_error
    └── EngineError               also thrown directly ("Could not open the window")
        ├── MissingComponentError        the entity does not have the requested component
        ├── EntityLimitError             more than MAX_ENTITIES entities alive, or an out of
        │                                range id given to the EntityManager
        ├── RegisterError                the same system registered twice
        ├── ComponentNotRegisteredError  component or system not registered
        ├── ResourceNotFoundError        the resource was never added to the scene
        └── OutOfRangeEntityError        the entity id is not below MAX_ENTITIES`;
</script>

<svelte:head>
	<title>How the game loop works - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How the game loop works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How the client starts, runs the schedules on a fixed time step and draws the game
</p>

<p class="max-w-[100ch] leading-7">
	The client is driven by the {@render c('Application')} class
	({@render c('src/game-rtype/Application.hpp')}). It owns the {@render c('Scene')}, adds the plugins
	that open the window and connect to the server, then runs the game loop until
	{@render c('AppAlive')} becomes false. The client does not simulate the game on its own: the server
	is authoritative, the client sends its inputs and draws the entities of the snapshots it receives.
</p>

<h2 class="mt-9 text-lg font-semibold">Startup</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('main')} ({@render c('src/main.cpp')}) reads the optional arguments
	{@render c('[ip] [port]')}, which default to {@render c('rtype_protocol::DEFAULT_ADDRESS')} and
	{@render c('rtype_protocol::DEFAULT_PORT')}. With one argument, it is the port. Too many arguments,
	an invalid IPv4 address or a port outside 1-65535 prints an error and exits with code
	{@render c('84')} before the application is created. Then it creates the application and runs it.
	Any exception that reaches it is printed and the program exits with code {@render c('84')}:
</p>
{@render code(mainExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	The constructor registers the base components and the {@render c('AppAlive')} resource, then adds the
	plugins, in the order required by the ECS (a component must be registered before a plugin uses it):
</p>
{@render code(constructorExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		<strong class="font-semibold">Plugins</strong>: a plugin implements {@render c('IPlugin')}
		({@render c('build')} and {@render c('cleanup')}). {@render c('addPlugin')} stores it and calls
		its {@render c('build')} right away; {@render c('run')} calls every {@render c('cleanup')} once
		the loop ends. See
		<a href="/developer/ecs-systems" class="text-[#ff8a3d] hover:underline">How systems work</a> for
		the systems each plugin registers.
	</li>
	<li>
		<strong class="font-semibold">RayLibRenderer</strong> opens the window (up to 1920 x 1080, fitted to the monitor), registers
		{@render c('Sprite')}, {@render c('GetInput')} and {@render c('RenderSystem')}.
		<strong class="font-semibold">Physics</strong> registers {@render c('Velocity')},
		{@render c('Collider2D')}, {@render c('MovementSystem')} and {@render c('Collider2DSystem')}.
		<strong class="font-semibold">Client</strong> adds the {@render c('Socket')} resource and the
		network systems.
	</li>
	<li>
		<strong class="font-semibold">No entity is created</strong> at startup: {@render c('ReadSocket')}
		creates them from the server snapshots. Their {@render c('Sprite')} gets the shape, size, color and
		draw order of the snapshot, and a texture chosen from the type, the variant and the player id.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">The fixed time step</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The {@render c('FixedUpdate')} schedule always advances by steps of exactly
	{@render c('FIXED_DELTA_TIME')}, 1/60 of a second (a {@render c('constexpr')} in
	{@render c('src/engine/core/Time.hpp')}), whatever the speed of the computer. The other schedules run
	once per frame. The frame time is measured with {@render c('std::chrono::steady_clock')}:
</p>
{@render code(loopExample)}
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-5 leading-7">
	<li>The time spent on the last frame is added to an accumulator.</li>
	<li>{@render c('PrepUpdate')} and {@render c('StateTransition')} run once.</li>
	<li>
		As long as the accumulator holds at least one step, {@render c('FixedUpdate')} runs once, the
		entities queued with {@render c('queueDestroy')} are destroyed, and one step is removed from the
		accumulator. The rest is kept for the next frame.
	</li>
	<li>{@render c('Update')} then {@render c('PostUpdate')} run once; the frame is drawn there.</li>
</ol>
{@render code(stepsExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	A fixed step makes the simulation independent of the CPU speed and the frame rate, which is why the
	server uses the same {@render c('FIXED_DELTA_TIME')}. On the client the {@render c('FixedUpdate')}
	systems have no entity to move today, since the server runs the game. The frame time is clamped to {@render c('MAX_FRAME_TIME')} (0.25 s)
	so that a long freeze, such as dragging the window, does not trigger hundreds of steps at once.
</p>
<p class="mt-3 max-w-[100ch] leading-7">
	On the client, the schedules hold these systems (in registration order, see
	<a href="/developer/ecs" class="text-[#ff8a3d] hover:underline">How the ECS works</a> for how
	schedules are run):
</p>
{@render code(schedulesExample)}

<h2 class="mt-9 text-lg font-semibold">The renderer</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The engine never calls raylib for drawing. {@render c('RenderSystem')} draws through the
	{@render c('IRenderer')} interface ({@render c('src/plugins/Renderer/IRenderer.hpp')}), which it
	gets from the scene as a {@render c('std::reference_wrapper<IRenderer>')} resource. Colours use the
	engine's own {@render c('ColorRGBA')} type, defined in {@render c('Sprite.hpp')}:
</p>
{@render code(rendererExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('RayLibRenderer')} ({@render c('src/plugins/Renderer/RayLibRenderer.hpp')}) is both an
	{@render c('IPlugin')} and an {@render c('IRenderer')}. Its {@render c('build')} (not its
	constructor) hides raylib logs, opens the window and sets a 60 FPS target; its destructor unloads
	the textures and closes the window. {@render c('drawSprite')} draws the texture of the sprite,
	loaded once and cached ({@render c('getTexture')}), stretched to the shape and centred on the
	position. Without a texture it draws a {@render c('BoxShape')} as a rectangle and a
	{@render c('CircleShape')} as a circle.
</p>
<p class="mt-3 max-w-[100ch] leading-7">
	Changing the graphics library needs a new plugin implementing {@render c('IRenderer')}, but also a
	new {@render c('GetInput')} (it calls raylib to read the keyboard). The server also includes the
	Renderer {@render c('Sprite')} component and the raylib headers, see
	<a href="/developer/architecture" class="text-[#ff8a3d] hover:underline">Architecture</a>.
</p>

<h2 class="mt-9 text-lg font-semibold">Errors</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The engine reports misuse with exceptions defined in
	{@render c('src/engine/errors/EngineError.hpp')}. Most inherit from {@render c('EngineError')}, but
	{@render c('FileNotFound')}, {@render c('UnknownComponentType')} and
	{@render c('MultipleComponentDefinition')} inherit directly
	from {@render c('std::exception')}, so catching {@render c('EngineError')} misses them. All of them
	end up in the {@render c('catch')} of {@render c('main')} if nothing catches them before:
</p>
{@render code(errorsExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		{@render c('GetInput')} runs in {@render c('PrepUpdate')}: it reads the keyboard with raylib
		directly (ZQSD, arrows, Space) into the {@render c('KeyboardInput')} resource, so input does not
		go through {@render c('IRenderer')}. It also sets {@render c('AppAlive.alive')} to false when the
		window is closed.
	</li>
	<li>
		The loop also stops when {@render c('ReadSocket')} sets {@render c('AppAlive.alive')} to false:
		connection rejected, player eliminated by the server, or no snapshot received for too long. See
		<a href="/developer/network-disconnect" class="text-[#ff8a3d] hover:underline"
			>How disconnects and timeouts work</a
		>.
	</li>
	<li>
		{@render c('SendInput')} runs once per frame, not once per {@render c('FixedUpdate')} step. Details
		are in
		<a href="/developer/network-client" class="text-[#ff8a3d] hover:underline"
			>How the client network works</a
		>.
	</li>
	<li>
		When the loop ends, {@render c('Client::cleanup')} sends a {@render c('DISCONNECT')} packet if the
		client was connected, then closes the socket. The other plugins' {@render c('cleanup')} do nothing.
	</li>
	<li>
		The server runs its own loop ({@render c('GameLoop::run')} in
		{@render c('src/server/game/game_loop.cpp')}): it sleeps until the next tick, runs
		{@render c('FixedUpdate')} once, then flushes the destroy queue. Off-screen handling lives there:
		enemy, bullet and star scripts call {@render c('Game::DestroyOffScreenEntity')}, enemies also
		move with {@render c('Game::makeSinusoidalMovement')}, {@render c('Game::SpawnEnemy')} and
		{@render c('Game::SpawnSpace')} create the waves and the stars, and the player's script
		calls {@render c('Game::maintainPlayerOnScreen')} ({@render c('src/game-rtype/Game.cpp')}).
	</li>
	<li>
		The server times its ticks with the {@render c('TickTimeManager')} resource
		({@render c('src/engine/core/TickTimeManager.cpp')}). {@render c('getNextTickTime')} is computed
		from the start time as {@render c('start + (tick + 1) * FIXED_TICK_DURATION')}, where
		{@render c('FIXED_TICK_DURATION')} is exactly 1/60 s ({@render c('src/engine/core/Time.hpp')}). Since
		it never adds up small durations, no rounding error builds up, and a slow tick is caught up by
		the next ones. Each loop calls {@render c('advance()')} then {@render c('sleep_until(getNextTickTime())')}.
	</li>
</ul>
