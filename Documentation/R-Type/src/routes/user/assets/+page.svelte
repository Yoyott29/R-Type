<svelte:head><title>Assets and sprites - R-Type docs</title></svelte:head>
<h1 class="text-3xl font-semibold">Assets and sprites</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">How textures are selected and displayed</p>
<p class="max-w-[100ch] leading-7">Assets live in <code>Assets/</code>. Launch the client from the repository root because texture paths are relative to the process directory.</p>
<h2 class="mt-9 text-lg font-semibold">Current assets</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Players: <code>PlayerGreen.png</code>, <code>PlayerBlue.png</code>, <code>PlayerMagenta.png</code>, <code>PlayerRed.png</code></li>
	<li>Enemies: <code>Bydos1.png</code>, <code>Bydos2.png</code></li>
	<li>Projectile: <code>Bullet.png</code></li>
</ul>
<h2 class="mt-9 text-lg font-semibold">Sprite data flow</h2>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>The server creates a <code>Sprite</code> component.</li>
	<li>Snapshots send dimensions, shape, color, draw order, and variant.</li>
	<li>The client reconstructs the sprite and selects a local texture path.</li>
	<li><code>RayLibRenderer</code> loads textures lazily and scales them to the sprite rectangle.</li>
</ol>
<p class="mt-3 max-w-[100ch] leading-7">Missing files raise <code>FileNotFound</code> when the renderer loads them. Linux paths are case-sensitive.</p>
