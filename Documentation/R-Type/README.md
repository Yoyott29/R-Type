# R-Type documentation site

The source of the R-Type documentation, online at <https://r-type-project-documentation.vercel.app>. It is a [SvelteKit](https://svelte.dev/docs/kit) site styled with Tailwind CSS.

## Run it locally

Requires Node.js 20 or newer. From this folder:

```sh
npm install
npm run dev          # http://localhost:5173, reloads on save
npm run build        # production build
npm run preview      # serve the production build
```

## Layout

```
src/
├── app.html                  # HTML shell (lang="en")
├── lib/
│   ├── pages.js              # sidebar sections and pages, also used by the sitemap
│   ├── site.js               # public URL of the site
│   └── components/           # Sidebar, RTypeLogo
└── routes/
    ├── +layout.svelte        # sidebar, skip link, main area
    ├── +page.svelte          # home page
    ├── user/<page>/          # user documentation
    ├── developer/<page>/     # developer documentation
    ├── sitemap.xml/          # generated sitemap
    └── robots.txt/           # generated robots.txt
```

## Add a page

1. Create `src/routes/user/<name>/+page.svelte` or `src/routes/developer/<name>/+page.svelte`.
2. Register it in `src/lib/pages.js` with its title, path and section.

The page then shows in the sidebar and the sitemap. The full guide is the [How to add a page](https://r-type-project-documentation.vercel.app/developer/add-a-page) page.

## Deployment

Every push to `main` that changes this folder deploys the site to Vercel production, through `.github/workflows/deploy.yml`. See [Deploy Documentation](https://r-type-project-documentation.vercel.app/developer/documentation-deployment).
