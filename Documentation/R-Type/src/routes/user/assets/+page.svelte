<script>
	const code0 = `{
    "texture": "Bydos1Anim.png",
    "frameWidth": 21.0,
    "frameHeight": 24,
    "frames": 8,
    "fps": 12,
    "loop": true
}`;
</script>

<svelte:head>
	<title>Assets and Sprites - R-Type docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Assets and Sprites</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">The images, animations and sounds of the game</p>

<p class="mt-3 max-w-[100ch] leading-7">
	Every file of the game is in <code>R-Type/assets/</code>. Entities point to them by name in <code>R-Type/R-Type.gringo</code>, the scene edited with the <a href="/gringo/getting-started" class="text-accent hover:underline">Gringo editor</a>.
</p>

<h2 class="mt-9 text-lg font-semibold">Images</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Files</th><th class="px-4 py-3">Used for</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>PlayerBlue.png</code>, <code>PlayerGreen.png</code>, <code>PlayerMagenta.png</code>, <code>PlayerRed.png</code></td><td class="px-4 py-3 align-top">The four ships, P1 to P4</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Bydos1Anim.png</code>, <code>Bydos2Anim.png</code> (and <code>Bydos1.png</code>, <code>Bydos2.png</code>)</td><td class="px-4 py-3 align-top">The two kinds of enemies</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>PowArmor.png</code></td><td class="px-4 py-3 align-top">The power-up carrier</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>PlayerShot.png</code>, <code>PlayerShotUp.png</code>, <code>PlayerShotDown.png</code>, <code>Bullet.png</code></td><td class="px-4 py-3 align-top">The ship's shots, straight and diagonal</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>BulletCharge1.png</code> to <code>BulletCharge5.png</code></td><td class="px-4 py-3 align-top">The charged shot, one per level</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>EnemyBullet.png</code></td><td class="px-4 py-3 align-top">The enemies' shots</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>PowerUp.png</code>, <code>powerups.png</code></td><td class="px-4 py-3 align-top">The power-up</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Explosion.png</code></td><td class="px-4 py-3 align-top">The explosion of a destroyed enemy</td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">Animations</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	A sprite sheet holds the frames side by side. A <code>.anim</code> file next to it says how to play it, for example <code>Bydos1Anim.anim</code>:
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code0}</code></pre>

<p class="mt-3 max-w-[100ch] leading-7">
	An entity plays it with an <code>Animator</code> component. The editor's Animation panel creates <code>.anim</code> files, see <a href="/gringo/animations-and-scripts" class="text-accent hover:underline">Animations and Scripts</a>.
</p>

<h2 class="mt-9 text-lg font-semibold">Sound and font</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><code>stage1.mp3</code>: the stage music, played during the match.</li>
	<li><code>Font.ttf</code>: the font of the texts and buttons.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Each file is loaded once, the first time it is drawn or played, then kept.</li>
	<li>An image is drawn centered on its entity, at the size of its sprite.</li>
	<li>A missing file stops the client with <code>File not found: ...</code>. Linux paths are case-sensitive.</li>
	<li>Only add files you are allowed to redistribute.</li>
</ul>
