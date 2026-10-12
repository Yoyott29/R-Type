<script>
	const mainExample = `int main()
{
    try {
        Application application;
        application.run();
    } catch (std::exception const &error) {
        std::cerr << "Fatal error: " << error.what() << "\\n";
        return 84;
    }
    return 0;
}`;

	const constructorExample = `Application::Application() : _renderer(std::make_unique<RaylibRenderer>())
{
    _renderer->init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE); // 1200 x 800, "R-Type"
    setupComponents(); // register Position, Velocity, Sprite, PlayerControlled
    setupSystems();    // register the 3 systems and their requirements
    setupEntities();   // create the player and a decoration
}`;

	const loopExample = `#define FIXED_DELTA_TIME (1.0f / 60.0f)
#define MAX_FRAME_TIME 0.25f

void Application::run()
{
    float accumulator = 0.0f;

    while (!_renderer->shouldClose()) {
        float frameTime = _renderer->getFrameTime();
        if (frameTime > MAX_FRAME_TIME)
            frameTime = MAX_FRAME_TIME;
        accumulator += frameTime;

        while (accumulator >= FIXED_DELTA_TIME) {
            _inputSystem->update(_scene);
            _movementSystem->update(_scene);
            accumulator -= FIXED_DELTA_TIME;
        }

        _renderer->beginFrame();
        _renderer->clear(ColorRGBA{0, 0, 0, 255});
        _renderSystem->update(_scene, *_renderer);
        _renderer->endFrame();
    }
}`;

	const stepsExample = `frame takes 1/60 s   -> 1 simulation step,  1 render
frame takes 1/30 s   -> 2 simulation steps, 1 render
frame takes 1 s      -> clamped to 0.25 s, about 15 simulation steps, 1 render`;

	const rendererExample = `class IRenderer {
  public:
    virtual ~IRenderer() = default;
    virtual void init(int width, int height, std::string const &title) = 0;
    virtual bool shouldClose() = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void clear(ColorRGBA color) = 0;
    virtual void drawRectangle(float x, float y, float width, float height, ColorRGBA color) = 0;
    virtual float getFrameTime() = 0;
};`;

	const errorsExample = `std::runtime_error
└── EngineError
    ├── MissingComponentError        the entity does not have the requested component
    ├── EntityLimitError             more than MAX_ENTITIES entities alive
    ├── ComponentNotRegisteredError  the component type was not registered
    ├── OutOfRangeEntityError        the entity id is not below MAX_ENTITIES
    └── RegisterError                defined, not thrown anywhere yet`;
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
	How the client starts, runs the systems on a fixed time step and draws the game
</p>

<p class="max-w-[100ch] leading-7">
	The client is driven by the {@render c('Application')} class
	({@render c('src/game-rtype/Application.hpp')}). It creates the window, sets up the ECS, then runs
	the game loop until the window is closed. For now the whole game runs locally, without the server.
</p>

<h2 class="mt-9 text-lg font-semibold">Startup</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('main')} creates the application and runs it. Any exception that reaches it is printed
	and the program exits with code {@render c('84')}:
</p>
{@render code(mainExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	The constructor opens the window and prepares the {@render c('Scene')}, in the order required by
	the ECS:
</p>
{@render code(constructorExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		<strong class="font-semibold">The player</strong>: a red 50 x 35 rectangle at (0, 200), with
		{@render c('Position')}, {@render c('Velocity')}, {@render c('Sprite')} and
		{@render c('PlayerControlled')}. It moves with the arrow keys.
	</li>
	<li>
		<strong class="font-semibold">The decoration</strong>: a blue 64 x 16 rectangle at (400, 100), with
		a constant velocity of (100, 20) pixels per second. It has no
		{@render c('PlayerControlled')}, so the keyboard does not affect it.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">The fixed time step</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The simulation (input and movement) always advances by steps of exactly
	{@render c('FIXED_DELTA_TIME')}, 1/60 of a second ({@render c('src/engine/core/Time.hpp')}), whatever
	the speed of the computer. Drawing happens once per frame.
</p>
{@render code(loopExample)}
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-5 leading-7">
	<li>The time spent on the last frame is added to an accumulator.</li>
	<li>
		As long as the accumulator holds at least one step, the simulation systems run once and one step
		is removed from it. The rest is kept for the next frame.
	</li>
	<li>The frame is drawn with the render system.</li>
</ol>
{@render code(stepsExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	A fixed step keeps the movement identical on every machine, and the same simulation can later run on
	the server. The frame time is clamped to {@render c('MAX_FRAME_TIME')} (0.25 s) so that a long freeze,
	such as dragging the window, does not trigger hundreds of steps at once.
</p>

<h2 class="mt-9 text-lg font-semibold">The renderer</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The engine never calls raylib. It draws through the {@render c('IRenderer')} interface
	({@render c('src/engine/renderer/IRenderer.hpp')}), with its own colour type
	{@render c('ColorRGBA')}:
</p>
{@render code(rendererExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('RaylibRenderer')} ({@render c('src/game-rtype/RaylibRenderer.hpp')}) implements this
	interface with raylib. It opens the window with a 60 FPS target, hides raylib logs, converts
	{@render c('ColorRGBA')} to raylib colours and closes the window in its destructor. Changing the
	graphics library only requires a new class implementing {@render c('IRenderer')}.
</p>

<h2 class="mt-9 text-lg font-semibold">Errors</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The engine reports misuse with exceptions defined in
	{@render c('src/engine/errors/EngineError.hpp')}. They all inherit from {@render c('EngineError')},
	so they can be caught together, and they end up in the {@render c('catch')} of {@render c('main')}
	if nothing catches them before:
</p>
{@render code(errorsExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		{@render c('InputSystem')} reads the keyboard with raylib directly, so input does not go through
		{@render c('IRenderer')}.
	</li>
	<li>
		Systems run in the order they are called in {@render c('run')}: input first, then movement, so a
		key press moves the player in the same step.
	</li>
	<li>Nothing keeps entities on screen yet: the decoration keeps moving and leaves the window.</li>
</ul>
