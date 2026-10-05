<script>
	const triggerExample = `on:
  push:
    branches: [main]
    paths:
      - 'Documentation/R-Type/**'`;

	const conditionExample = `jobs:
  deploy:
    if: github.repository != 'EpitechPGE3-2026/G-ING-500-LIL-5-1-rtype-4'
    runs-on: ubuntu-latest
    defaults:
      run:
        working-directory: Documentation/R-Type`;

	const envExample = `env:
  VERCEL_ORG_ID: \${{ secrets.VERCEL_ORG_ID }}
  VERCEL_PROJECT_ID: \${{ secrets.VERCEL_PROJECT_ID }}`;

	const stepsExample = `- run: npm i -g vercel
- run: vercel pull --yes --environment=production --token=\${{ secrets.VERCEL_TOKEN }}
- run: vercel build --prod --token=\${{ secrets.VERCEL_TOKEN }}
- run: vercel deploy --prebuilt --prod --token=\${{ secrets.VERCEL_TOKEN }}`;
</script>

<svelte:head>
	<title>Deploy Docs - R-Type docs</title>
</svelte:head>

{#snippet code(text)}
	<pre
		class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code
			>{text}</code
		></pre>
{/snippet}

{#snippet c(text)}<code
		class="rounded border border-[#1e2b26] bg-[#121a17] px-1.5 py-0.5 font-mono text-[13.5px]"
		>{text}</code
	>{/snippet}

<h1 class="text-3xl font-semibold">Deploy Docs</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How this documentation site is built and published to Vercel
</p>

<p class="max-w-[68ch] leading-7">
	The workflow {@render c('.github/workflows/deploy-docs.yml')} builds the site in
	{@render c('Documentation/R-Type')} and deploys it to production on Vercel. It runs on its own
	whenever the documentation changes.
</p>

<h2 class="mt-9 text-lg font-semibold">When it runs</h2>
<p class="mt-3 max-w-[68ch] leading-7">
	The workflow starts on every push to <strong class="font-semibold">main</strong> that changes a
	file inside {@render c('Documentation/R-Type')}. Pushes that only touch other folders do not
	trigger a deploy.
</p>
{@render code(triggerExample)}

<h2 class="mt-9 text-lg font-semibold">Where it runs</h2>
<ul class="mt-3 max-w-[68ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		{@render c('if')} skips the job on the original school repository, so only other copies of the
		repository run the deploy.
	</li>
	<li>{@render c('ubuntu-latest')} is a standard GitHub-hosted machine.</li>
	<li>
		{@render c('working-directory')} makes every {@render c('run')} step execute inside the docs
		folder.
	</li>
</ul>
{@render code(conditionExample)}

<h2 class="mt-9 text-lg font-semibold">Secrets</h2>
<p class="mt-3 max-w-[68ch] leading-7">
	The deploy needs three repository secrets, set in
	<strong class="font-semibold">Settings &gt; Secrets and variables &gt; Actions</strong>:
</p>
<ul class="mt-3 max-w-[68ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>{@render c('VERCEL_TOKEN')}: your Vercel access token.</li>
	<li>{@render c('VERCEL_ORG_ID')}: the ID of your Vercel account or team.</li>
	<li>{@render c('VERCEL_PROJECT_ID')}: the ID of the Vercel project to deploy.</li>
</ul>
{@render code(envExample)}

<h2 class="mt-9 text-lg font-semibold">Steps</h2>
<p class="mt-3 max-w-[68ch] leading-7">
	After checking out the code and installing Node 20, the workflow runs four commands:
</p>
<ul class="mt-3 max-w-[68ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>{@render c('npm i -g vercel')} installs the Vercel CLI.</li>
	<li>{@render c('vercel pull')} downloads the project settings and environment variables.</li>
	<li>{@render c('vercel build --prod')} builds the site on the runner.</li>
	<li>
		{@render c('vercel deploy --prebuilt --prod')} uploads that build to production without
		rebuilding it on Vercel.
	</li>
</ul>
{@render code(stepsExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[68ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>It only deploys from main, and always to production. There are no preview deployments.</li>
	<li>If a secret is missing, the {@render c('vercel pull')} step fails and nothing is deployed.</li>
	<li>You can not start it manually, because there is no {@render c('workflow_dispatch')} trigger.</li>
</ul>