<script>
	const code0 = `#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"

inline const std::vector<std::string> Mover_params = {"speed"};

class Mover : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "speed")
            _speed = std::stof(value);
    }

    void onStart(Scene &, Entity &) override {}

    void onUpdate(Scene &scene, Entity &entity) override
    {
        scene.getComponent<Transform>(entity).x += _speed * FIXED_DELTA_TIME;
    }

  private:
    float _speed = 100.0f;
};`;
	const code1 = `const std::optional<Entity> target = scene.getResource<EditorEntityIds>().find(_targetId);`;
</script>

<svelte:head>
	<title>Scripts - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Scripts</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Gameplay code in C++: GringoEntityScript, parameters and button scripts</p>

<p class="mt-3 max-w-[100ch] leading-7">
	A script is a C++ class in a header of the project's <code>scripts/</code> folder. It is compiled into the game, not interpreted: there is no scripting language, and changing a script means building the game again.
</p>

<h2 class="mt-9 text-lg font-semibold">An entity script</h2>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code0}</code></pre>

<h2 class="mt-9 text-lg font-semibold">The rules</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><strong>The file name is the class name</strong>: <code>Mover.hpp</code> must declare <code>class Mover</code>. The generated main writes <code>std::make_shared&lt;Mover&gt;()</code>.</li>
	<li>It derives from <code>GringoEntityScript</code> and defines <code>onStart</code> and <code>onUpdate</code> (the build checks both names are in the file).</li>
	<li><code>onStart</code> runs once, before the first <code>onUpdate</code>; <code>onUpdate</code> runs every fixed tick (60 per second), so use <code>FIXED_DELTA_TIME</code> for movements.</li>
	<li>Each Script component gets its <strong>own instance</strong>: member variables are per entity. Every copy of a prefab gets its own instance too.</li>
	<li>An entity can have several Script components.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Parameters</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	The line <code>&lt;ClassName&gt;_params</code> lists the parameters. The editor reads it from the file (the names between the braces, even on several lines) and shows one value per name under the Script component. When the game starts, the generated main calls <code>setParameter(name, value)</code> for each one, with the value as text, before <code>onStart</code>.
</p>

<p class="mt-3 max-w-[100ch] leading-7">
	A parameter can hold the editor id of another entity; find the entity with the <code>EditorEntityIds</code> resource:
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code1}</code></pre>

<h2 class="mt-9 text-lg font-semibold">Button scripts</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	A <code>Button</code> component (UI plugin) has its own <code>Script</code> value. Its class derives from <code>IGringoButtonScript</code> and defines <code>onClick(Scene &amp;, Entity)</code>, called by the ButtonClickSystem. It can declare parameters the same way.
</p>

<h2 class="mt-9 text-lg font-semibold">Writing a script for a networked game</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	With the Network plugin, the same script is compiled into the server and the client. Test the side with the <code>BuildVariantName</code> resource (<code>"server"</code> or <code>"client"</code>) and keep the game rules on the server. The R-Type and TrackNTrap scripts are examples: see the <a href="/trackntrap/developer/architecture" class="text-accent hover:underline">TrackNTrap developer documentation</a>.
</p>
