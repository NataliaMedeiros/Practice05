#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

void put_nbr(int nb)
{
	char n;

	if(nb >= 10)
		put_nbr(nb / 10);
	n = (nb % 10) + '0';
	write(1, &n, 1);
}

int error_clean(int fd, char *buffer)
{
	if(fd != -1)
		close(fd);
	if(buffer)
		free(buffer);
	write(1, "Map error\n", 10);
	return (1);
}

char *read_file(int capacity, int fd)
{
	char	*buffer;
	char	chunk[1024];
	int		n;
	int		i;
	int		j;

	n = 0;
	i = 0;
	j = 0;
	buffer = malloc((capacity + 1) * sizeof(char));
	if(!buffer)
		return(NULL);
	while((n = read(fd, chunk, 1024)) > 0)
	{
		j = 0;
		while(j < n)
		{
			buffer[i] = chunk[j];
			i++;
			j++;
		}
	}
	if (n == -1)
		return (free(buffer), NULL);
	buffer[i] = '\0';
	return(buffer);
}

int is_buffer_valid(char *buffer)
{
	int i;
	int w;
	int start;

	i = 0;
	w = -1;
	start = 0;
	while(buffer[i] != '\0')
	{
		if(buffer[i] == '\n')
		{
			if(w == -1)
				w = i;
			else if (i - start != w)
				return (1);
			start = i + 1;
		}
		if (buffer[i] != '.' && buffer[i] != 'X' && buffer[i] != '\n')
			return (1);
		i++;
	}
	if (w == -1 || w == 0|| (i != start && i - start != w))
		return (1);
	return (0);
}

void flood_fill(char *buffer, int i, int capacity, int w, int *counter)
{
	if (i < 0 || i >= capacity || buffer[i] == '\0' || buffer[i] != 'X')
		return ;
	buffer[i] = '0';
	(*counter)++;
	flood_fill(buffer, i + 1, capacity, w, counter);
	flood_fill(buffer, i - 1, capacity, w, counter);
	flood_fill(buffer, i + w + 1, capacity, w, counter);
	flood_fill(buffer, i - w - 1, capacity, w, counter);
}

int get_width(char *buffer)
{
	int i;

	i = 0;
	while (buffer[i] != '\n')
		i++;
	return (i);
}

int find_largest_island(char *buffer, int capacity)
{
	int	larger;
	int island_size;
	int	i;
	int w;

	larger = 0;
	island_size = 0;
	i = 0;
	w = get_width(buffer);
	while(buffer[i] != '\0')
	{
		if (buffer[i] == 'X')
		{
			island_size = 0;
			flood_fill(buffer, i, capacity, w, &island_size);
			if (island_size > larger)
				larger = island_size;
			break ;
		}
		i++;
	}
	return (larger);
}

int main(int argc, char **argv)
{
	int		fd;
	int		n;
	int		capacity;
	char	chunk[1024];
	char	*buffer;
	int		largest_island;

	n = 0;
	capacity = 0;
	largest_island = 0;
	if(argc != 2)
		return(error_clean(-1, NULL));
	fd = open(argv[1], O_RDONLY);
	if(fd == -1)
		return(error_clean(-1, NULL));
	while((n = read(fd, chunk, 1024)) > 0)
		capacity += n;
	close(fd);
	fd = open(argv[1], O_RDONLY);
	if(fd == -1)
		return(error_clean(-1, NULL));
	buffer = read_file(capacity, fd);
	if(!buffer)
		return(error_clean(fd, NULL));
	if (is_buffer_valid(buffer) == 1)
		return(error_clean(fd, buffer));
	largest_island = find_largest_island(buffer, capacity);
	put_nbr(largest_island);
	write(1, "\n", 1);
	close(fd);
	free(buffer);
	return (0);
}
