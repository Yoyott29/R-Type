<svelte:head><title>Assets and sprites - R-Type docs</title></svelte:head>
<h1 class="text-3xl font-semibold">Assets and sprites</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">How textures are selected and displayed</p>
<p class="max-w-[100ch] leading-7">Assets live in <code>Assets/</code>. Launch the client from the repository root because texture paths are relative to the process directory.</p>
<h2 class="mt-9 text-lg font-semibold">Current assets</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Players: <code>PlayerGreen.png</code>, <code>PlayerBlue.png</code>, <code>PlayerMagenta.png</code>, <code>PlayerRed.png</code></li>
	<li>Enemies: <code>Bydos1.png</code>, <code>Bydos2.png</code></li>
	<li>Projectile: <code>Bullet.png</code></li>
	<li>Stars: no texture, drawn as 5 x 5 white squares.</li>
</ul>
<h2 class="mt-9 text-lg font-semibold">Which texture is used</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>A player uses the texture of its slot: 0 green, 1 blue, 2 magenta, 3 red.</li>
	<li>An enemy uses <code>Bydos2.png</code> when its variant is 1, <code>Bydos1.png</code> otherwise.</li>
	<li>A bullet uses <code>Bullet.png</code>.</li>
	<li>Any other entity has no texture and is drawn as a plain box or circle of its color.</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">This choice is made on the client, in <code>playerTextures</code> and <code>texturePathForType</code> in <code>src/plugins/Network/Client/systems/ReadSocket.cpp</code>. To add a texture, follow <a href="/developer/how-to" class="text-[#ff8a3d] hover:underline">How-to guides</a>.</p>
<h2 class="mt-9 text-lg font-semibold">Sprite data flow</h2>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>The server creates a <code>Sprite</code> component.</li>
	<li>Snapshots send dimensions, shape, color, draw order, and variant.</li>
	<li>The client reconstructs the sprite and selects a local texture path.</li>
	<li><code>RayLibRenderer</code> loads textures lazily and scales them to the sprite rectangle.</li>
</ol>
<p class="mt-3 max-w-[100ch] leading-7">A texture is drawn centred on the entity position and stretched to the size of its sprite. Entities are drawn by increasing draw order, so stars (draw order -1) stay behind everything else.</p>
<p class="mt-3 max-w-[100ch] leading-7">Missing files raise <code>FileNotFound</code> when the renderer loads them, and the client stops with <code>Fatal error: File not found: ...</code>. Linux paths are case-sensitive.</p>
