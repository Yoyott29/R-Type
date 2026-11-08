<svelte:head>
	<title>Entities and Components - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Entities and Components</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Fill your game and set its values</p>

<h2 class="mt-9 text-lg font-semibold">Add entities and components</h2>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Click <strong>Add Entity</strong> (or Add Prefab), then click the new item to select it.</li>
	<li>
		Its components appear in the middle. Every entity has a <code>Transform</code> (its position,
		rotation and scale), which cannot be removed.
	</li>
	<li>
		Click <strong>+</strong> to add a component, chosen in the list of the active plugins
		(<code>Cancel</code> closes the list). The small button on a component's header removes it.
	</li>
</ol>

<h2 class="mt-9 text-lg font-semibold">Manage an entity</h2>
<p class="mt-3 max-w-[100ch] leading-7">Over the components of the selected entity or prefab:</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><code>Rename</code>: change its name.</li>
	<li>Its id: the number <code>ChildOf</code> uses to pick a parent (prefabs have none).</li>
	<li><code>Up</code> / <code>Down</code>: move it in the list.</li>
	<li><code>Delete</code>: click once, then again on <code>Confirm?</code> to delete it.</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">
	To make an entity the child of another one, add a <code>ChildOf</code> component and set its
	<code>Parent</code> to the parent's id: the child then moves with its parent.
</p>

<h2 class="mt-9 text-lg font-semibold">Edit the values</h2>
<p class="mt-3 max-w-[100ch] leading-7">Click a value to change it. What happens depends on its type:</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>
		<strong>Numbers and texts</strong>: type the new value (<code>Backspace</code> and the arrow keys
		work), then press <code>Enter</code>.
	</li>
	<li><strong>Choices, True / False and colors</strong>: each click shows the next value.</li>
	<li>
		<strong>Files</strong> (an image, a sound, a script...): each click shows the next file of the
		right type in the project's <code>assets/</code> or <code>scripts/</code> folder. Copy your files
		there first.
	</li>
</ul>

<p class="mt-9 max-w-[100ch] leading-7">
	Next: <a href="/gringo/animations-and-scripts" class="text-accent hover:underline">Animations and Scripts</a>.
</p>
