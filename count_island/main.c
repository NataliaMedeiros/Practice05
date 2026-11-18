#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int error_clean(int fd, char *buffer)
{
	if(fd != -1)
		close(fd);
	if(buffer)
		free(buffer);
	write(1, "\n", 1);
	return (1);
}

int file_length(int fd)
{
	int		capacity;
	int		n;
	char	chunk[1024];

	capacity = 0;
	while((n = read(fd, chunk, 1024)) > 0)
		capacity += n;
	if (n == -1 || capacity == 0)
		return(-1);
	return(capacity);
}

int read_file(int fd, char *buffer)
{
	char	chunk[1024];
	int		n;
	int		j;
	int		i;

	i = 0;
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
	buffer[i] = '\0';
	return (n);
}

int is_map_valid(char *buffer)
{
	int		i;
	int		start;
	int		w;

	i = 0;
	start = 0;
	w = -1;
	while(buffer[i] != '\0')
	{
		start = i;
		while(buffer[i] && buffer[i] != '\n')
		{
			if (buffer[i] != 'X' && buffer[i] != '.')
				return (0);
			i++;
		}
		if (w == -1)
			w = i;
		if (i - start != w)
			return (0);
		if(buffer[i])
			i++;
	}
	if (i == 0 || (buffer[i] == '\0' && buffer[i - 1] != '\n'))
		return (0);
	return (w);
}

void	flood_fill(char *buffer, int i, int w, char island, int total_length)
{
	if (i < 0 || i >= total_length || buffer[i] == '\0')
		return ;
	if (buffer[i] != 'X')
		return ;
	buffer[i] = island;
	flood_fill(buffer, i + 1, w, island, total_length);
	flood_fill(buffer, i - 1, w, island, total_length);
	flood_fill(buffer, i + w + 1, w, island, total_length);
	flood_fill(buffer, i - w - 1, w, island, total_length);
}

int main(int argc, char **argv)
{
	//1 - validar argumentos
	//2 - abrir arquivo
	//3 - ler arquivo para o buffer
	//4 - validar buffer
	//5 - flood fill
	//6 - imprimir
	//7 - limpar
	int		fd;
	char	*buffer;
	int		total_length;
	int		n;
	int		w;
	int		i;
	int		island;

	fd = 0;
	w = -1;
	i = 0;
	island = 0;
	if (argc != 2)
		return(error_clean(-1, NULL));
	fd = open(argv[1], O_RDONLY);
	if(fd == -1)
		return(error_clean(-1, NULL));
	total_length = file_length(fd);
	if (total_length == -1)
		return (error_clean(fd, NULL));
	close(fd);
	fd = open(argv[1], O_RDONLY);
	if(fd == -1)
		return(error_clean(-1, NULL));
	buffer = malloc((total_length + 1) * sizeof(char));
	if (!buffer)
		return(error_clean(fd, NULL));
	n = read_file(fd, buffer);
	if(n == -1)
		return(error_clean(fd, buffer));
	w = is_map_valid(buffer);
	if(w == 0)
		return(error_clean(fd, buffer));
	while(buffer[i] != '\0')
	{
		if(buffer[i] == 'X')
		{
			flood_fill(buffer, i, w, island + '0', total_length);
			island++;
		}
		i++;
	}
	write(1, buffer, total_length);
	close(fd);
	free(buffer);
	return (0);
}
