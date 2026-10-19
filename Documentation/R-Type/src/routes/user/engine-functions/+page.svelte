<svelte:head><title>Engine functions - R-Type docs</title></svelte:head>
<h1 class="text-3xl font-semibold">Engine functions</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Using the ECS engine from game code</p>
<p class="max-w-[100ch] leading-7">The engine is centered around <code>Scene</code>. Register types and systems before creating entities.</p>
<h2 class="mt-9 text-lg font-semibold">Entities and components</h2>
<pre class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>scene.registerComponent&lt;Position&gt;();
Entity ship = scene.createEntity();
scene.addComponent(ship, Position&#123;100.0f, 200.0f&#125;);
Position &amp;position = scene.getComponent&lt;Position&gt;(ship);
scene.queueDestroy(ship);
scene.flushDestroyQueue();</code></pre>
<p class="mt-3 max-w-[100ch] leading-7">Use <code>isEntityAlive</code> before accessing components through stored ids. Ids are reused after an entity is destroyed and have no generation counter, so a stored id can point to a newer entity: drop stored ids when the entity is destroyed. Missing components throw an engine error.</p>
<h2 class="mt-9 text-lg font-semibold">Systems and schedules</h2>
<pre class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>scene.registerSystem&lt;MovementSystem&gt;(FixedUpdate);
ComponentSet required;
required.set(scene.getComponentType&lt;Position&gt;());
required.set(scene.getComponentType&lt;Velocity&gt;());
scene.setSystemComponentSet&lt;MovementSystem&gt;(required);
scene.runSchedule(FixedUpdate);</code></pre>
<p class="mt-3 max-w-[100ch] leading-7">Available schedules are <code>PrepUpdate</code>, <code>StateTransition</code>, <code>FixedUpdate</code>, <code>Update</code>, and <code>PostUpdate</code>.</p>
<h2 class="mt-9 text-lg font-semibold">Prefabs and resources</h2>
<pre class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>PrefabsManager prefabs;
prefabs.createPrefab("Enemy", Prefab()
    .addComponent(Position&#123;2000.0f, 0.0f&#125;));
Entity enemy = prefabs.InstantiatePrefab("Enemy", scene);

scene.addResource&lt;TickTimeManager&gt;();
TickTimeManager &amp;ticks = scene.getResource&lt;TickTimeManager&gt;();</code></pre>
<p class="mt-3 max-w-[100ch] leading-7">Instantiating a prefab name that was never created gives an empty entity, without any error. For step-by-step guides (a new component, system, prefab or plugin), see <a href="/developer/how-to" class="text-[#ff8a3d] hover:underline">How-to guides</a> in the developer documentation.</p>
