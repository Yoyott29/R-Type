// Served from a route: the root .gitignore ignores every *.txt file
import { SITE } from '$lib/site.js';

export const prerender = true;

export function GET() {
	const body = `User-agent: *
Allow: /

Sitemap: ${SITE}/sitemap.xml
`;
	return new Response(body, { headers: { 'Content-Type': 'text/plain' } });
}
