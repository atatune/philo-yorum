#include <sys/time.h> // Zaman işlemleri için gereken header dosyası
#include <unistd.h> // Unix standard fonksiyonlar için gereken header dosyası
#include "philo.h" // Projeye özel header dosyası

int	ft_atoi(const char *str)
{
	int			i; // Dizindeki mevcut konum
	int			tmp; // Geçici değişken

	i = 0;
	tmp = 0;
	while (str[i] <= 32 && str[i + 1]) // Boşluk ve kontrol karakterlerini atla
		i++;
	while (str[i] == '+') // Artı işaretlerini atla
		i++;
	if (str[i] == '-' || str[i] == 0) // Eksi işareti veya null karakter kontrolü
		return (-1);
	tmp = i;
	while (str[i]) // Karakter dizisinin sonuna kadar ilerle
	{
		if (!(str[i] >= '0' && str[i] <= '9')) // Sayısal olmayan karakter kontrolü
			return (-1);
		i++;
	}
	i = 0;
	while (str[tmp] >= '0' && str[tmp] <= '9' && str[tmp]) // Sayıları integer'a çevir
	{
		i = (i * 10) + (str[tmp] - '0'); // Her basamağı ekle
		tmp++;
	}
	return (i); // Sonuç olarak integer değeri döndür
}

u_int64_t	timestamp(void)
{
	struct timeval	time; // Zamanı tutmak için struct

	if (gettimeofday(&time, (void *)0) == -1) // Geçerli zamanı al
		return (1); // Hata durumunda 1 döndür
	return (time.tv_sec * 1000 + time.tv_usec / 1000); // Zamanı milisaniye olarak döndür
}

u_int64_t	time_diff(u_int64_t past, u_int64_t pres)
{
	return (pres - past); // Geçmiş ile şu an arasındaki farkı döndür
}

void	smart_sleep(u_int64_t time, t_rules *rules)
{
	u_int64_t	i; // Başlangıç zamanı

	i = timestamp(); // Geçerli zamanı al
	while (1)
	{
		if (time_diff(i, timestamp()) >= time) // Belirtilen süreyi kontrol et
			break ;
		pthread_mutex_lock(&(rules->died_check)); // Ölüm kontrol mutexini kilitle
		if (rules->died) // Eğer bir filozof öldüyse
		{
			pthread_mutex_unlock(&(rules->died_check)); // Mutex kilidini aç
			break ;
		}
		pthread_mutex_unlock(&(rules->died_check)); // Mutex kilidini aç
		usleep(50); // 50 mikro saniye bekle
	}
}
