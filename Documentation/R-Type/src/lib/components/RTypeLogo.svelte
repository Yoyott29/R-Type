<script>
	let { class: className = '' } = $props();
</script>

<svg
	viewBox="0 0 520 120"
	xmlns="http://www.w3.org/2000/svg"
	role="img"
	aria-label="R-Type"
	class={className}
>
	<defs>
		<linearGradient id="rt-chrome" gradientUnits="userSpaceOnUse" x1="0" y1="10" x2="0" y2="110">
			<stop offset="0" stop-color="#8ff0ff" />
			<stop offset="0.45" stop-color="#ffffff" />
			<stop offset="0.55" stop-color="#5bb8cc" />
			<stop offset="1" stop-color="#2b7f94" />
		</linearGradient>
		<filter id="rt-glow" x="-20%" y="-40%" width="140%" height="180%">
			<feGaussianBlur stdDeviation="5" result="blur" />
			<feMerge>
				<feMergeNode in="blur" />
				<feMergeNode in="SourceGraphic" />
			</feMerge>
		</filter>
	</defs>

	<g
		fill="none"
		stroke="url(#rt-chrome)"
		stroke-width="14"
		stroke-linecap="square"
		stroke-linejoin="miter"
		filter="url(#rt-glow)"
	>
		<!-- R -->
		<path transform="translate(10 10)" d="M0 100V0H48Q78 0 78 28Q78 56 48 56H0M42 56L80 100" />
		<!-- T -->
		<path transform="translate(138 10)" d="M0 0H70M35 0V100" />
		<!-- Y -->
		<path transform="translate(236 10)" d="M0 0L35 50L70 0M35 50V100" />
		<!-- P -->
		<path transform="translate(334 10)" d="M0 100V0H48Q78 0 78 28Q78 56 48 56H0" />
		<!-- E -->
		<path transform="translate(440 10)" d="M70 0H0V100H70M0 50H55" />
	</g>

	<!-- the dot between R and TYPE -->
	<circle cx="114" cy="60" r="9" fill="none" stroke="url(#rt-chrome)" stroke-width="5" />
	<circle cx="114" cy="60" r="3" fill="#ff8a3d" />
</svg>