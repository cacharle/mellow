#ifndef MALLOC_H
# define MALLOC_H

# include <unistd.h>
# include <sys/mman.h>
# include <sys/mman.h>
# include <stddef.h>

void	*malloc(size_t size);
void	free(void *ptr);
void	*realloc(void *ptr, size_t size);

typedef struct
{
	size_t	size;
	void	*data;
}

#endif
