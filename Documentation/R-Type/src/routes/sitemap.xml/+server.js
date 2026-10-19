import { userPages, developerPages } from '$lib/pages.js';
import { SITE } from '$lib/site.js';

export const prerender = true;

export function GET() {
	const paths = ['/', ...userPages.map((p) => p.path), ...developerPages.map((p) => p.path)];
	const urls = paths.map((path) => `  <url><loc>${SITE}${path}</loc></url>`).join('\n');
	const body = `<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
${urls}
</urlset>
`;
	return new Response(body, { headers: { 'Content-Type': 'application/xml' } });
}
