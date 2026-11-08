<script>
	const code0 = `class IPlugin
{
  public:
    virtual void registerComponents(Scene &scene) { (void)scene; }
    virtual void build(Application &app) = 0;
    virtual void cleanup(Application &app) = 0;

    virtual bool requiresRenderer() const { return false; }
    virtual std::vector<PropertyDescriptor> settings() const { return {}; }
    virtual std::vector<BuildVariant> buildVariants() const { return {}; }
};`;
	const code1 = `class Audio : public Plugin<Audio>
{
  public:
    void registerComponents(Scene &scene) override;
    void build(Application &app) override;
    void cleanup(Application &app) override;
    bool requiresRenderer() const override { return true; }
};`;
	const code2 = `template <> struct ComponentDescription<AudioSource> {
    static inline const bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"AudioSource",
                {
                    {"Path", valueType::File, NO_FILE, {"assets", ".mp3"}},
                    {"Volume", valueType::Int, "100"},
                    {"Loop", valueType::Bool, "False"},
                    {"PlayOnStart", valueType::Bool, "True"},
                },
                {58, 53, 63, 255},   // color of its card in the editor
                ""};
    }
};`;
	const code3 = `scene.registerComponent<AudioSource>(
    [](Scene &target, Entity entity, const nlohmann::json &data) {
        AudioSource source;
        const std::string path = ComponentJson::value(data, "Path", NO_FILE);
        if (path != NO_FILE)
            source.path = ProjectAssets::resolve(target, path);
        source.volume = ComponentJson::intValue(data, "Volume", 100);
        source.loop = ComponentJson::boolValue(data, "Loop", false);
        if (ComponentJson::boolValue(data, "PlayOnStart", true))
            source.play();
        target.addComponent(entity, source);
    });`;
	const code4 = `void Audio::build(Application &app)
{
    app._scene.registerSystem<AudioSystem>(Update);
    ComponentSet audioRequirements;
    audioRequirements.set(app._scene.getComponentType<AudioSource>());
    app._scene.setSystemComponentSet<AudioSystem>(audioRequirements);
    app._scene.setSystemAccess<AudioSystem>(app._scene.access().read<AudioSource>());
}`;
</script>

<svelte:head>
	<title>How plugins work - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">How plugins work</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">IPlugin, Plugin&lt;T&gt;, the PluginLoader, and writing a new plugin</p>

<p class="mt-3 max-w-[100ch] leading-7">
	Every feature beyond the ECS core is a plugin: a shared library in <code>Gringo Engine/plugins/</code>, opened by name when a game starts. A game lists its plugins in its <code>.gringo</code> file.
</p>

<h2 class="mt-9 text-lg font-semibold">The interface</h2>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code0}</code></pre>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Function</th><th class="px-4 py-3">When</th><th class="px-4 py-3">What to do in it</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>registerComponents</code></td><td class="px-4 py-3 align-top">Loading a game, and in the editor</td><td class="px-4 py-3 align-top">Register the plugin's components, with a function reading each one from the <code>.gringo</code> JSON. Nothing else: the editor calls it too</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>build</code></td><td class="px-4 py-3 align-top">Loading a game, after registerComponents</td><td class="px-4 py-3 align-top">Add resources and systems, read the settings, open sockets...</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>cleanup</code></td><td class="px-4 py-3 align-top">When the game loop ends</td><td class="px-4 py-3 align-top">Close what <code>build</code> opened</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>requiresRenderer</code></td><td class="px-4 py-3 align-top">Building and starting a game</td><td class="px-4 py-3 align-top">True if the plugin draws or reads the keyboard: a headless server leaves it out, and a game without renderer refuses to start with it</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>settings</code></td><td class="px-4 py-3 align-top">In the editor</td><td class="px-4 py-3 align-top">The project settings of the plugin, shown in the Settings panel; read them in <code>build</code> from the <code>ProjectSettings</code> resource</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>buildVariants</code></td><td class="px-4 py-3 align-top">Building a game</td><td class="px-4 py-3 align-top">Extra executables: Network returns a headless <code>server</code> and a <code>client</code></td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">How a library becomes a plugin</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	A plugin class derives from <code>Plugin&lt;Derived&gt;</code> instead of <code>IPlugin</code>. <code>Plugin&lt;T&gt;</code> holds a static variable whose initialisation registers a <code>create</code> function with the <code>PluginLoader</code>, keyed by the library file. That variable is initialised when the library is opened, so a plugin needs no exported function.
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code1}</code></pre>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>PluginLoader::load("Audio")</code> opens <code>Audio.so</code> (or <code>Audio.dll</code>) and calls the <code>create</code> function registered for that file. Libraries are never closed: components and systems made by a plugin keep running its code until the program ends.
</p>

<h2 class="mt-9 text-lg font-semibold">Where the plugins are found</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	The loader looks next to the running executable, in this order, and takes the first folder that exists:
</p>

<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li><code>&lt;exe folder&gt;/plugins</code></li>
	<li><code>&lt;exe folder&gt;/../plugins</code></li>
	<li><code>&lt;exe folder&gt;/Gringo Engine/plugins</code></li>
	<li><code>&lt;exe folder&gt;/../Gringo Engine/plugins</code></li>
</ol>

<p class="mt-3 max-w-[100ch] leading-7">
	This is why R-Type (at the repository root) and TrackNTrap (in <code>Gringo Engine/TrackNTrap</code>) both find the same plugins. Renderers are looked up the same way in <code>renderers/</code>.
</p>

<h2 class="mt-9 text-lg font-semibold">What happens to a plugin</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><strong>In a game</strong>, <code>Application::addPlugin(name)</code> loads it, adds its default settings to <code>ProjectSettings</code>, then calls <code>registerComponents</code> and <code>build</code>. When the loop ends, <code>cleanup</code> is called on every plugin.</li>
	<li><strong>In the editor</strong>, the <code>ComponentCatalog</code> loads every library of <code>plugins/</code> and only calls <code>registerComponents</code> and <code>settings</code>. Each component found is tagged with its plugin: the editor offers it only while that plugin is active, and a build fails if a component's plugin is off.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Making a component visible to the editor</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	A component appears in the editor if it has a <code>ComponentDescription</code>. Its properties are what the editor shows and saves:
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code2}</code></pre>

<p class="mt-3 max-w-[100ch] leading-7">
	Property types: <code>Int</code>, <code>Float</code>, <code>String</code>, <code>Bool</code>, <code>Color</code>, <code>Choice</code> (one of the options), <code>File</code> (options: folder then extensions) and <code>Prefab</code>. A property can be shown only when another one has a value, like Collider2D's <code>Width</code> only for a <code>Box</code> shape.
</p>

<p class="mt-3 max-w-[100ch] leading-7">
	The game reads those properties back in the function given to <code>registerComponent</code>, with the <code>ComponentJson</code> helpers:
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code3}</code></pre>

<h2 class="mt-9 text-lg font-semibold">Write a plugin</h2>

<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Create <code>src/plugins/MyPlugin/</code> with a class deriving from <code>Plugin&lt;MyPlugin&gt;</code>. The folder name is the plugin name.</li>
	<li>Write its components, each with a <code>ComponentDescription</code> if the editor should offer it.</li>
	<li>Write its systems (see <a href="/gringo/developer/ecs-systems" class="text-accent hover:underline">How systems work</a>).</li>
	<li>In <code>registerComponents</code>, register each component with its JSON reader.</li>
	<li>In <code>build</code>, register each system on a schedule, give it its required components with <code>setSystemComponentSet</code>, and declare its accesses with <code>setSystemAccess</code> so it can run in parallel with others.</li>
	<li>Override <code>requiresRenderer</code> if it draws, and <code>settings</code> if the project should configure it.</li>
	<li>Build the engine again: the new folder becomes <code>plugins/MyPlugin.so</code>, and the editor shows it in its Plugins panel.</li>
</ol>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code4}</code></pre>

<p class="mt-3 max-w-[100ch] leading-7">
	Next: <a href="/gringo/developer/built-in-plugins" class="text-accent hover:underline">The built-in plugins</a>.
</p>
