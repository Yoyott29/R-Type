<svelte:head>
	<title>Renderers - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Renderers</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">IRenderer: the only door to the graphics library</p>

<p class="mt-3 max-w-[100ch] leading-7">
	No plugin and no game calls raylib. They call an <code>IRenderer</code>, which the application puts in the scene as a resource (<code>std::reference_wrapper&lt;IRenderer&gt;</code>). The renderer is a library of <code>Gringo Engine/renderers/</code>, chosen by the project (<code>"renderer"</code> in the <code>.gringo</code>), loaded like a plugin.
</p>

<h2 class="mt-9 text-lg font-semibold">What a renderer provides</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Group</th><th class="px-4 py-3">Functions</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Window</td><td class="px-4 py-3 align-top"><code>openWindow</code>, <code>closeWindow</code>, <code>isWindowOpen</code>, <code>beginFrame</code>, <code>endFrame</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">2D drawing</td><td class="px-4 py-3 align-top"><code>drawRectangle</code>, <code>drawCircle</code>, <code>drawText</code>, <code>drawImage</code>, <code>drawImageRegion</code> (a frame of a sprite sheet)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">3D drawing</td><td class="px-4 py-3 align-top"><code>drawModel3D</code>, with a <code>RendererCamera3D</code> and a <code>RendererModel3D</code> (file or primitive, color, texture, scale, rotation, animation)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Input</td><td class="px-4 py-3 align-top"><code>isKeyDown</code>, <code>getMousePosition</code>, <code>getMouseDelta</code>, <code>isMouseButtonPressed</code>, <code>setCursorLocked</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Accessibility</td><td class="px-4 py-3 align-top"><code>setColorFilter</code>: none, protanopia, deuteranopia or tritanopia</td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">Rules every renderer follows</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The game draws in a fixed world, the size given to <code>openWindow</code> (1920 by 1080): the renderer scales and centers it in the window, whatever the window size. The mouse position is given in world coordinates.</li>
	<li>Every <code>(x, y)</code> given to a draw function is the <strong>center</strong> of what is drawn.</li>
	<li>Files are given by path: the renderer loads each texture, font or model once and keeps it.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">RaylibRenderer</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	The only renderer today, in <code>src/renderers/RaylibRenderer/</code>. It caches textures, fonts, models and animations, and applies the color filter with a shader: when a filter is on, the frame is drawn into a texture, then drawn to the screen through the shader.
</p>

<h2 class="mt-9 text-lg font-semibold">Add a renderer</h2>

<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Create <code>src/renderers/MyRenderer/</code> with a class deriving from <code>Renderer&lt;MyRenderer&gt;</code> and implementing every <code>IRenderer</code> function.</li>
	<li>Build the engine: it becomes <code>renderers/MyRenderer.so</code>. The CMake rules give every renderer the raylib library; another graphics library needs its own lines there.</li>
	<li>Choose it for a project in the editor (Settings panel, or the name screen of a new project).</li>
</ol>

<p class="mt-3 max-w-[100ch] leading-7">
	Next: <a href="/gringo/developer/network" class="text-accent hover:underline">How the network works</a>.
</p>
