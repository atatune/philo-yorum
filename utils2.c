#include "philo.h" // Projeye özel header dosyası
#include <unistd.h> // Unix standard fonksiyonlar için gereken header dosyası
#include <stdlib.h> // Standart kütüphane fonksiyonları için gereken header dosyası
#include <stdio.h> // Standart input-output fonksiyonları için gereken header dosyası

void	action_print(t_rules *rules, int id, char *string)
{
	pthread_mutex_lock(&(rules->died_check)); // Ölüm kontrol mutex'ini kilitle
	if (!(rules->died)) // Eğer bir filozof ölmediyse
	{
		printf("%lli ", timestamp() - rules->first_timestamp); // Zaman damgası ve başlangıç zamanından geçen süreyi yazdır
		printf("%i ", id); // Filozof ID'sini yazdır
		printf("%s\n", string); // Verilen metni yazdır
		usleep(50); // 50 mikro saniye bekle
	}
	pthread_mutex_unlock(&(rules->died_check)); // Mutex kilidini aç
	return ; // Fonksiyondan çık
}

int	write_error(char *str)
{
	int	len; // String'in uzunluğu

	len = 0;
	while (str[len]) // String'in uzunluğunu hesapla
		len++;
	write(2, "Error: ", 7); // Hata mesajı başlığı yazdır
	write(2, str, len); // Hata mesajını yazdır
	write(2, "\n", 1); // Yeni satır karakteri yazdır
	return (1); // Hata durumu döndür
}

void	ft_free(t_rules *rules)
{
	free (rules->forks); // Çatal mutexlerini serbest bırak
	free (rules->philosophers); // Filozofları serbest bırak
}

void	eat_control(t_rules *r, t_philosopher *p)
{
	int	i; // Döngü değişkeni

	i = 0;
	pthread_mutex_lock(&(r->x_ate_check)); // Yeme kontrol mutex'ini kilitle
	while (r->nb_eat != -1 && i < r->nb_philo && p[i].x_ate == r->nb_eat) // Yemek yeme sayısını kontrol et
		i++;
	pthread_mutex_unlock(&(r->x_ate_check)); // Mutex kilidini aç
	if (i == r->nb_philo) // Eğer tüm filozoflar belirtilen sayıda yemek yedi ise
	{
		pthread_mutex_lock(&(r->all_ate_check)); // Tüm yeme kontrol mutex'ini kilitle
		r->all_ate = 1; // Tüm filozofların yediği flag'ini işaretle
		pthread_mutex_unlock(&(r->all_ate_check)); // Mutex kilidini aç
	}
}

void	philo_eats(t_philosopher *philo)
{
	t_rules	*rules; // Kurallar yapısına işaretçi

	rules = philo->rules; // Filozofun kurallar yapısına işaretçisini ata
	pthread_mutex_lock(&(rules->forks[philo->left_fork_id])); // Sol çatal mutex'ini kilitle
	action_print(rules, philo->id, "has taken a fork"); // Eylemi yazdır
	pthread_mutex_lock(&(rules->forks[philo->right_fork_id])); // Sağ çatal mutex'ini kilitle
	action_print(rules, philo->id, "has taken a fork"); // Eylemi yazdır
	pthread_mutex_lock(&(rules->meal_check)); // Yemek kontrol mutex'ini kilitle
	action_print(rules, philo->id, "is eating"); // Eylemi yazdır
	pthread_mutex_unlock(&(rules->meal_check)); // Mutex kilidini aç
	pthread_mutex_lock(&(rules->t_last_meal_check)); // Son yemek yeme kontrol mutex'ini kilitle
	philo->t_last_meal = timestamp(); // Filozofun son yemek yediği zamanı ata
	pthread_mutex_unlock(&(rules->t_last_meal_check)); // Mutex kilidini aç
	smart_sleep(rules->time_eat, rules); // Akıllı uyuma fonksiyonunu çağır
	pthread_mutex_lock(&(rules->x_ate_check)); // Yeme kontrol mutex'ini kilitle
	(philo->x_ate)++; // Filozofun yemek yeme sayısını arttır
	pthread_mutex_unlock(&(rules->x_ate_check)); // Mutex kilidini aç
	pthread_mutex_unlock(&(rules->forks[philo->left_fork_id])); // Sol çatal mutex'ini serbest bırak
	pthread_mutex_unlock(&(rules->forks[philo->right_fork_id])); // Sağ çatal mutex'ini serbest bırak
}
