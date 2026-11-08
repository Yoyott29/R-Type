<svelte:head>
	<title>Animations and Scripts - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Animations and Scripts</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Animate sprites and write gameplay code</p>

<h2 class="mt-9 text-lg font-semibold">Animations</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The <strong>Animation</strong> panel turns a sprite sheet (the frames side by side in one PNG
	image, in <code>assets/</code>) into an animation:
</p>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Click <strong>Animation</strong> in the top bar.</li>
	<li>Give the animation a name.</li>
	<li>Click <code>Image</code> until it shows your sprite sheet.</li>
	<li>Type the number of frames in the image.</li>
	<li>Choose if it loops, then click <strong>CREATE</strong>.</li>
</ol>
<p class="mt-3 max-w-[100ch] leading-7">
	The animation is saved as a <code>.anim</code> file in <code>assets/</code> and plays at 12 frames
	per second. To play it, add an <code>Animator</code> component (2D plugin) to an entity and pick it
	in its <code>Animation</code> value.
</p>

<h2 class="mt-9 text-lg font-semibold">Scripts</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Scripts are the gameplay code of your entities: C++ files (<code>.hpp</code>) in the project's
	<code>scripts/</code> folder. They need the Scripts plugin.
</p>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Write the script in <code>scripts/</code>. It defines <code>onStart</code> (run once) and <code>onUpdate</code> (run every game tick).</li>
	<li>Add a <code>Script</code> component to an entity or a prefab.</li>
	<li>Click its <code>Path</code> until it shows your file.</li>
	<li>The parameters the script declares appear under it, as values to edit like any other.</li>
</ol>
<p class="mt-3 max-w-[100ch] leading-7">
	An entity can have several scripts. A <code>Button</code> (UI plugin) also takes a script, which
	defines <code>onClick</code>.
</p>
<p class="mt-3 max-w-[100ch] leading-7">
	The <a href="/developer/how-to" class="text-accent hover:underline">How-to guides</a> of the R-Type
	developer documentation show how to write scripts and components.
</p>

<p class="mt-9 max-w-[100ch] leading-7">
	Next: <a href="/gringo/building-and-running" class="text-accent hover:underline">Building and Running</a>.
</p>
