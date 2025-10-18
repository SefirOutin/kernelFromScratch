#ifndef _CONSOLE_H
#define _CONSOLE_H

#include "type.h"

/*
 * Interface Console — abstraction d’une console texte.
 * 
 * Toute implémentation (ex : VGA, série, framebuffer)
 * doit définir une instance de ConsoleVTable et un type
 * concret dont le premier membre est struct Console.
 */

struct console_vtable {
	void (*putchar)(void *self, char c);
	void (*clear)(void *self);
};

struct console {
	const struct console_vtable *vptr;
};

/* --- Inline helpers --- */
/*
 * console_putchar - écrit un caractère sur la console.
 * @self: instance concrète (VGA, série, etc.)
 * @c: caractère à afficher
 */
static inline void console_putchar(struct console *self, char c)
{
	self->vptr->putchar(self, c);
}

/*
 * console_clear - efface le contenu de la console.
 * @self: instance concrète (VGA, série, etc.)
 */
static inline void console_clear(struct console *self)
{
	self->vptr->clear(self);
}

#endif /* _CONSOLE_H */
