// Sidebar sections and pages. `section` is the index in the matching sections list.
// A page listed here is also added to the sitemap.

export const userSections = [
	{ title: 'Introduction' },
	{ title: 'Playing R-Type' },
	{ title: 'Reference' }
];

export const developerSections = [
	{ title: 'Introduction' },
	{ title: 'Developer Environment' },
	{ title: 'Build and Tests' },
	{ title: 'GitHub Workflows' },
	{ title: 'Guides' },
	{ title: 'Documentation' }
];

export const userPages = [
	{ title: 'About', path: '/user/about', section: 0 },
	{ title: 'Getting Started', path: '/user/getting-started', section: 0 },
	{ title: 'Controls', path: '/user/controls', section: 1 },
	{ title: 'Gameplay', path: '/user/gameplay', section: 1 },
	{ title: 'Multiplayer', path: '/user/multiplayer', section: 1 },
	{ title: 'Assets and Sprites', path: '/user/assets', section: 2 },
	{ title: 'Troubleshooting', path: '/user/troubleshooting', section: 2 },
	{ title: 'Accessibility', path: '/user/accessibility', section: 2 }
];

export const developerPages = [
	{ title: 'About', path: '/developer/about', section: 0 },
	{ title: 'Architecture', path: '/developer/architecture', section: 0 },
	{ title: 'Network protocol', path: '/developer/protocol', section: 0 },
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

	{ title: 'How-to guides', path: '/developer/how-to', section: 4 },

	{ title: 'How to add a page', path: '/developer/add-a-page', section: 5 }
];

export const trackntrapUserSections = [{ title: 'Introduction' }, { title: 'Playing TrackNTrap' }];

export const trackntrapUserPages = [
	{ title: 'About', path: '/trackntrap/about', section: 0 },
	{ title: 'Getting Started', path: '/trackntrap/getting-started', section: 0 },
	{ title: 'Controls', path: '/trackntrap/controls', section: 1 },
	{ title: 'Gameplay', path: '/trackntrap/gameplay', section: 1 },
	{ title: 'Multiplayer', path: '/trackntrap/multiplayer', section: 1 }
];

export const trackntrapDeveloperSections = [
	{ title: 'Introduction' },
	{ title: 'Race Logic' },
	{ title: 'Players' },
	{ title: 'Track' },
	{ title: 'Guides' }
];

export const trackntrapDeveloperPages = [
	{ title: 'Architecture', path: '/trackntrap/developer/architecture', section: 0 },
	{ title: 'The Project File', path: '/trackntrap/developer/project-file', section: 0 },
	{ title: 'Shared State', path: '/trackntrap/developer/shared-state', section: 1 },
	{ title: 'Race Flow', path: '/trackntrap/developer/race-flow', section: 1 },
	{ title: 'The Rat', path: '/trackntrap/developer/rat', section: 2 },
	{ title: 'Cameras', path: '/trackntrap/developer/cameras', section: 2 },
	{ title: 'Mouse Traps', path: '/trackntrap/developer/traps', section: 2 },
	{ title: 'Obstacles', path: '/trackntrap/developer/obstacles', section: 3 },
	{ title: 'Cheese', path: '/trackntrap/developer/cheese', section: 3 },
	{ title: 'How to Modify', path: '/trackntrap/developer/how-to-modify', section: 4 }
];

// The engine pages (ECS, network) used to be in the R-Type developer documentation
export const gringoDeveloperSections = [
	{ title: 'Introduction' },
	{ title: 'ECS' },
	{ title: 'Plugins' },
	{ title: 'Network' },
	{ title: 'Editor' }
];

export const gringoDeveloperPages = [
	{ title: 'Architecture', path: '/gringo/developer/architecture', section: 0 },
	{ title: 'Building the Engine', path: '/gringo/developer/building', section: 0 },

	{ title: 'How the ECS works', path: '/gringo/developer/ecs', section: 1 },
	{ title: 'How entities work', path: '/gringo/developer/ecs-entities', section: 1 },
	{ title: 'How components work', path: '/gringo/developer/ecs-components', section: 1 },
	{ title: 'How systems work', path: '/gringo/developer/ecs-systems', section: 1 },
	{ title: 'How the game loop works', path: '/gringo/developer/game-loop', section: 1 },

	{ title: 'How plugins work', path: '/gringo/developer/plugins', section: 2 },
	{ title: 'The built-in plugins', path: '/gringo/developer/built-in-plugins', section: 2 },
	{ title: 'Renderers', path: '/gringo/developer/renderers', section: 2 },

	{ title: 'How the network works', path: '/gringo/developer/network', section: 3 },
	{ title: 'How the protocol works', path: '/gringo/developer/network-protocol', section: 3 },
	{ title: 'How replication works', path: '/gringo/developer/network-replication', section: 3 },
	{ title: 'How players and inputs work', path: '/gringo/developer/network-players', section: 3 },

	{ title: 'How the editor works', path: '/gringo/developer/editor', section: 4 },
	{ title: 'Scripts', path: '/gringo/developer/scripts', section: 4 }
];

export const gringoUserSections = [{ title: 'Introduction' }, { title: 'Using the Editor' }];

export const gringoUserPages = [
	{ title: 'About', path: '/gringo/about', section: 0 },
	{ title: 'Getting Started', path: '/gringo/getting-started', section: 0 },
	{ title: 'Creating a Project', path: '/gringo/creating-a-project', section: 1 },
	{ title: 'The Editor Window', path: '/gringo/editor-window', section: 1 },
	{ title: 'Plugins and Settings', path: '/gringo/plugins-and-settings', section: 1 },
	{ title: 'Entities and Components', path: '/gringo/entities-and-components', section: 1 },
	{ title: 'Animations and Scripts', path: '/gringo/animations-and-scripts', section: 1 },
	{ title: 'Building and Running', path: '/gringo/building-and-running', section: 1 }
];

// The sidebar of one project, in User or Developer mode
export function sidebarFor(project, mode) {
	if (project === 'TrackNTrap')
		return mode === 'User'
			? { sections: trackntrapUserSections, pages: trackntrapUserPages }
			: { sections: trackntrapDeveloperSections, pages: trackntrapDeveloperPages };
	if (project === 'Gringo')
		return mode === 'User'
			? { sections: gringoUserSections, pages: gringoUserPages }
			: { sections: gringoDeveloperSections, pages: gringoDeveloperPages };
	return mode === 'User'
		? { sections: userSections, pages: userPages }
		: { sections: developerSections, pages: developerPages };
}

// The project a page belongs to, from its path
export function projectOf(path) {
	if (path.startsWith('/trackntrap/')) return 'TrackNTrap';
	if (path.startsWith('/gringo/')) return 'Gringo';
	if (path === '/') return null;
	return 'R-Type';
}

// Every documented page, for the sitemap
export const allPages = [
	...userPages,
	...developerPages,
	...trackntrapUserPages,
	...trackntrapDeveloperPages,
	...gringoUserPages,
	...gringoDeveloperPages
];
