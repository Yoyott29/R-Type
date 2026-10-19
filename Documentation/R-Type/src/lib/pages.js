// Sidebar sections and pages. `section` is the index in the matching sections list.
// A page listed here is also added to the sitemap.

export const userSections = [
	{ title: 'Introduction' },
	{ title: 'Playing R-Type' },
	{ title: 'Using the Engine' },
	{ title: 'Reference' }
];

export const developerSections = [
	{ title: 'Introduction' },
	{ title: 'Developer Environment' },
	{ title: 'Build and Tests' },
	{ title: 'GitHub Workflows' },
	{ title: 'ECS' },
	{ title: 'Network' },
	{ title: 'Documentation' }
];

export const userPages = [
	{ title: 'About', path: '/user/about', section: 0 },
	{ title: 'Getting Started', path: '/user/getting-started', section: 0 },
	{ title: 'Controls', path: '/user/controls', section: 1 },
	{ title: 'Gameplay', path: '/user/gameplay', section: 1 },
	{ title: 'Multiplayer', path: '/user/multiplayer', section: 1 },
	{ title: 'Engine Functions', path: '/user/engine-functions', section: 2 },
	{ title: 'Assets and Sprites', path: '/user/assets', section: 2 },
	{ title: 'Troubleshooting', path: '/user/troubleshooting', section: 3 },
	{ title: 'Accessibility', path: '/user/accessibility', section: 3 }
];

export const developerPages = [
	{ title: 'About', path: '/developer/about', section: 0 },
	{ title: 'Architecture', path: '/developer/architecture', section: 0 },
	{ title: 'Technical study', path: '/developer/technical-study', section: 0 },
	{ title: 'Contributing', path: '/developer/contributing', section: 0 },

	{ title: 'Developer Setup', path: '/developer/developer-setup', section: 1 },
	{ title: '.githooks', path: '/developer/githooks', section: 1 },
	{ title: 'clang-format', path: '/developer/clang-format', section: 1 },
	{ title: 'clang-tidy', path: '/developer/clang-tidy', section: 1 },

	{ title: 'How CMakeLists works', path: '/developer/cmakelists', section: 2 },
	{ title: 'How CPM works', path: '/developer/cpm', section: 2 },
	{ title: 'How tests.sh works', path: '/developer/tests-sh', section: 2 },

	{ title: 'Auto Versioning', path: '/developer/auto-versioning', section: 3 },
	{ title: 'Deploy Documentation', path: '/developer/documentation-deployment', section: 3 },
	{ title: 'Mirror Repository', path: '/developer/mirror-repository', section: 3 },
	{ title: 'Running Tests', path: '/developer/running-tests', section: 3 },

	{ title: 'How the ECS works', path: '/developer/ecs', section: 4 },
	{ title: 'How entities work', path: '/developer/ecs-entities', section: 4 },
	{ title: 'How components work', path: '/developer/ecs-components', section: 4 },
	{ title: 'How systems work', path: '/developer/ecs-systems', section: 4 },
	{ title: 'How the game loop works', path: '/developer/game-loop', section: 4 },
	{ title: 'How-to guides', path: '/developer/how-to', section: 4 },

	{ title: 'How the server works', path: '/developer/udp-server', section: 5 },
	{ title: 'How the protocol works', path: '/developer/network-protocol', section: 5 },
	{ title: 'How connecting works', path: '/developer/network-connection', section: 5 },
	{ title: 'How snapshots work', path: '/developer/network-snapshots', section: 5 },
	{ title: 'How inputs work', path: '/developer/network-inputs', section: 5 },
	{ title: 'How disconnects and timeouts work', path: '/developer/network-disconnect', section: 5 },
	{ title: 'How the server threads work', path: '/developer/network-threads', section: 5 },
	{ title: 'How the client network works', path: '/developer/network-client', section: 5 },

	{ title: 'How to add a page', path: '/developer/add-a-page', section: 6 }
];
