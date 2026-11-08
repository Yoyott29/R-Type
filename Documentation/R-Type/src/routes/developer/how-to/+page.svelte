<script>
	import LinuxOnly from '$lib/components/LinuxOnly.svelte';
	import Platform from '$lib/components/Platform.svelte';

	const pre =
		'mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6';
	const winRegen = `& ".\\Gringo Engine\\Gringo.exe" --build R-Type\\R-Type.gringo   # regenerates the mains
cmake --build build --config Release --target r-type_client r-type_server -j`;
	const winCode = `cmake -S . -B build -A x64
cmake --build build --config Release --target r-type_client r-type_server -j`;
	const code0 = `./Gringo\\ Engine/Gringo --build R-Type/R-Type.gringo   # regenerates the mains
./build_rtype.sh
./tests/tests.sh`;
	const winOpen = `cd "Gringo Engine"
.\\Gringo.exe`;
	const linuxOpen = `cd "Gringo Engine"
./Gringo`;
</script>

<svelte:head>
	<title>How-to guides - R-Type docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">How-to guides</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Step by step recipes for the most common changes to R-Type</p>

<p class="mt-3 max-w-[100ch] leading-7">
	R-Type is changed through its scene (<code>R-Type/R-Type.gringo</code>, edited with the Gringo editor) and its scripts (<code>R-Type/scripts/</code>). After any change:
</p>

<Platform os="windows">
	<p class="mt-3 leading-7">From a <strong>Developer PowerShell for VS 2022</strong>, at the root of the repository (the second command expects the <code>build</code> folder configured as in <a href="/user/getting-started" class="text-accent hover:underline">Getting started</a>):</p>
	<pre class={pre}><code>{winRegen}</code></pre>
	<p class="mt-3 leading-7"><code>tests/tests.sh</code> is Linux only; run the engine unit tests as described in <a href="/developer/running-tests" class="text-accent hover:underline">Tests</a>.</p>
</Platform>

<Platform os="linux">
	<pre class={pre}><code>{code0}</code></pre>
	<LinuxOnly />
</Platform>

<p class="mt-3 max-w-[100ch] leading-7">
	To open the scene in the editor, start it from the <code>Gringo Engine</code> folder, then click <strong>Load Game</strong> and <code>R-Type</code>:
</p>

<Platform os="windows">
	<pre class={pre}><code>{winOpen}</code></pre>
</Platform>

<Platform os="linux">
	<pre class={pre}><code>{linuxOpen}</code></pre>
</Platform>

<p class="mt-5 max-w-[100ch] leading-7">
	See the <a href="/gringo/creating-a-project" class="text-accent hover:underline">Gringo user documentation</a> for the editor itself.
</p>

<h2 class="mt-9 text-lg font-semibold">Tune the game</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">To change</th><th class="px-4 py-3">Where</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Ship speed</td><td class="px-4 py-3 align-top">Each ship prefab: PlayerScript <code>speed</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Fire rate, charge time, screen size</td><td class="px-4 py-3 align-top">Constants at the top of <code>PlayerScript.hpp</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Enemy speed, size, look</td><td class="px-4 py-3 align-top">The Enemy and Enemy2 prefabs: Velocity, Collider2D, Sprite, Animator</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Wave size and rhythm</td><td class="px-4 py-3 align-top"><code>EnemySpawnerScript::onUpdate</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Enemy fire rate and bullet speed</td><td class="px-4 py-3 align-top">Constants of <code>EnemyShootScript.hpp</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Power-up chance</td><td class="px-4 py-3 align-top">EnemyScript <code>PowerUpChance</code> on each enemy prefab (1 = always, 1000 = one in a thousand)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Points needed for a POW armor</td><td class="px-4 py-3 align-top"><code>SCORE_STEP</code> in <code>PowArmorSpawnerScript.hpp</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Points per enemy</td><td class="px-4 py-3 align-top"><code>BulletScript.hpp</code> and <code>BulletChargeScript.hpp</code></td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">Add a kind of enemy</h2>

<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Put its sprite sheet in <code>R-Type/assets/</code> and create its <code>.anim</code> with the editor's Animation panel.</li>
	<li>In the editor, copy the Enemy prefab's components into a new prefab: Transform (<code>PositionX</code> 2000, off screen), Velocity, Collider2D (layer 2, trigger), Sprite, Animator with the new animation, <code>Networked</code> (Replicated, with the prefab's name), and the scripts <code>DestroyOffScreenScript</code>, <code>EnemyScript</code> and, if it shoots, <code>EnemyShootScript</code>.</li>
	<li>Spawn it: in <code>EnemySpawnerScript::onUpdate</code>, pick its name for some waves, as is done with <code>Enemy2</code>.</li>
</ol>

<h2 class="mt-9 text-lg font-semibold">Add a script</h2>

<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Create <code>R-Type/scripts/MyScript.hpp</code> with a class <code>MyScript</code> deriving from <code>GringoEntityScript</code> (the file and class names must match). Copy an existing script as a start.</li>
	<li>Declare its parameters in <code>MyScript_params</code> and read them in <code>setParameter</code>.</li>
	<li>Start <code>onStart</code> and <code>onUpdate</code> with a side test: the game rules go on the server (<code>if (isClientSide(scene)) return;</code>).</li>
	<li>Add a Script component to an entity or a prefab in the editor and pick the file.</li>
</ol>

<p class="mt-3 max-w-[100ch] leading-7">
	The rules for scripts are in <a href="/gringo/developer/scripts" class="text-accent hover:underline">Scripts</a>.
</p>

<h2 class="mt-9 text-lg font-semibold">Add a power-up</h2>

<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Create a prefab like PowerUp: Collider2D on layer 16 reacting to layer 1, Replicated, and a new script whose <code>onCollision</code> changes the ship.</li>
	<li>Give <code>PlayerScript</code> a method for the new effect, like <code>enableDiagonalShot</code>, and reset it in <code>revive</code>.</li>
	<li>Make enemies drop it: in <code>EnemyScript::explode</code>, spawn its prefab next to <code>"PowerUp"</code>.</li>
</ol>

<h2 class="mt-9 text-lg font-semibold">Add a sound</h2>

<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Put the <code>.mp3</code> in <code>R-Type/assets/</code>.</li>
	<li>For a sound tied to an entity, add an <code>AudioSource</code> to its prefab with <code>PlayOnStart</code> on (for example on <code>Explosion</code>, so every explosion plays it). The prefab must be replicated so the clients create it.</li>
	<li>For music, do as <code>StageMusic</code>: a replicated prefab with a looping AudioSource.</li>
</ol>

<h2 class="mt-9 text-lg font-semibold">Add a test</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	See <a href="/developer/tests-sh" class="text-accent hover:underline">How tests.sh works</a> for the end-to-end tests, and <a href="/gringo/developer/building" class="text-accent hover:underline">Building the Engine</a> for the engine unit tests.
</p>
