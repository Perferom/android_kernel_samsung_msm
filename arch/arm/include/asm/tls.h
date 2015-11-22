#ifndef __ASMARM_TLS_H
#define __ASMARM_TLS_H

#ifdef __ASSEMBLY__
	.macro set_tls_none, tp, tmp1, tmp2
	.endm

	.macro set_tls_v6k, tp, tmp1, tmp2
	mcr	p15, 0, \tp, c13, c0, 3		@ set TLS register
#if defined(CONFIG_TLS_USERSPACE_EMUL) && defined(CONFIG_CPU_USE_DOMAINS)
	mov	\tmp1, #0xffff0fff
	str	\tp, [\tmp1, #-15]		@ set TLS value at 0xffff0ff0
#elif defined(CONFIG_TLS_USERSPACE_EMUL) && !defined(CONFIG_CPU_USE_DOMAINS)
	ldr	\tmp1, =vectors_page
	ldr	\tmp1, [\tmp1]
	add	\tmp1, #0xff0
	str	\tp, [\tmp1]			@ set TLS value at vectors_page + 0xff0
#else
	mov	\tmp1, #0
	mcr	p15, 0, \tmp1, c13, c0, 2	@ clear user r/w TLS register
#endif
	.endm

	.macro set_tls_v6, tp, tmp1, tmp2
	ldr	\tmp1, =elf_hwcap
	ldr	\tmp1, [\tmp1, #0]
	mov	\tmp2, #0xffff0fff
	tst	\tmp1, #HWCAP_TLS		@ hardware TLS available?
	mcrne	p15, 0, \tp, c13, c0, 3		@ yes, set TLS register
#ifndef CONFIG_TLS_USERSPACE_EMUL
	movne	\tmp1, #0
	mcrne	p15, 0, \tmp1, c13, c0, 2	@ clear user r/w TLS register
	streq	\tp, [\tmp2, #-15]		@ set TLS value at 0xffff0ff0
#else
	str	\tp, [\tmp2, #-15]		@ set TLS value at 0xffff0ff0
#endif
	.endm

	.macro set_tls_software, tp, tmp1, tmp2
	mov	\tmp1, #0xffff0fff
	str	\tp, [\tmp1, #-15]		@ set TLS value at 0xffff0ff0
	.endm
#endif

#ifdef CONFIG_TLS_REG_EMUL
#define tls_emu		1
#define has_tls_reg		1
#define set_tls		set_tls_none
#elif defined(CONFIG_CPU_V6)
#define tls_emu		0
#define has_tls_reg		(elf_hwcap & HWCAP_TLS)
#define set_tls		set_tls_v6
#elif defined(CONFIG_CPU_32v6K)
#define tls_emu		0
#define has_tls_reg		1
#define set_tls		set_tls_v6k
#else
#define tls_emu		0
#define has_tls_reg		0
#define set_tls		set_tls_software
#endif

#endif	/* __ASMARM_TLS_H */
