#include "philo.h"
#include <unistd.h>
#include <stdio.h>

void routine_while(t_philosopher *philo, t_rules *rules)
{
	while (1)
	{
		philo_eats(philo); // Filozof yeme işlemini gerçekleştirir
		pthread_mutex_lock(&(rules->died_check)); // Ölüm kontrolü mutex'ini kilitler
		if (rules->died) // Eğer bir filozof öldüyse döngüden çıkar
		{
			pthread_mutex_unlock(&(rules->died_check)); // Ölüm kontrolü mutex'ini açar
			break;
		}
		pthread_mutex_unlock(&(rules->died_check)); // Ölüm kontrolü mutex'ini açar
		pthread_mutex_lock(&(rules->all_ate_check)); // Tüm filozoflar yemek yedi mi kontrol mutex'ini kilitler
		if (rules->nb_eat != -1 && (rules->all_ate || philo->x_ate >= rules->nb_eat)) // Eğer belirtilen yemek yeme sayısına ulaşıldıysa veya tüm filozoflar yemek yedi ise döngüden çıkar
		{
			pthread_mutex_unlock(&(rules->all_ate_check)); // Tüm filozoflar yemek yedi mi kontrol mutex'ini açar
			break;
		}
		pthread_mutex_unlock(&(rules->all_ate_check)); // Tüm filozoflar yemek yedi mi kontrol mutex'ini açar
		action_print(rules, philo->id, "is sleeping"); // Filozofun uyuduğunu yazdırır
		smart_sleep(rules->time_sleep, rules); // Akıllı uyuma fonksiyonunu çağırır
		action_print(rules, philo->id, "is thinking"); // Filozofun düşündüğünü yazdırır
	}
}

void *routine(void *void_philosopher)
{
	t_philosopher *philo;
	t_rules *rules;

	philo = (t_philosopher *)void_philosopher; // Void işaretçiyi filozof işaretçisine dönüştürür
	rules = philo->rules; // Filozofun kurallar yapısına işaretçisi
	if (!(philo->id % 2))
		usleep(15000); // Çift ID'li filozoflar için bekleme süresi ekler
	routine_while(philo, rules); // Ana döngü işlevini çağırır
	return (NULL); // NULL döner
}

void exit_launcher(t_rules *rules, t_philosopher *philos)
{
	int i;

	i = -1;
	while (++i < rules->nb_philo)
		pthread_join(philos[i].thread_id, NULL); // Tüm threadleri bekler
	i = -1;
	while (++i < rules->nb_philo)
		pthread_mutex_destroy(&(rules->forks[i])); // Tüm çatal mutex'lerini yok eder
	pthread_mutex_destroy(&(rules->die_check)); // Ölüm kontrol mutex'ini yok eder
	pthread_mutex_destroy(&(rules->meal_check)); // Yemek kontrol mutex'ini yok eder
	pthread_mutex_destroy(&(rules->x_ate_check)); // Yeme kontrol mutex'ini yok eder
	pthread_mutex_destroy(&(rules->t_last_meal_check)); // Son yemek zamanı kontrol mutex'ini yok eder
	pthread_mutex_destroy(&(rules->died_check)); // Ölüm kontrol mutex'ini yok eder
	pthread_mutex_destroy(&(rules->all_ate_check)); // Tüm yeme kontrol mutex'ini yok eder
	ft_free(rules); // Kurallar yapısını serbest bırakır
}

void death_checker(t_rules *r, t_philosopher *p, int i, int j)
{
	while (!(r->all_ate)) // Tüm filozoflar yemek yemediyse
	{
		i = -1;
		while (++i < r->nb_philo && !(r->died)) // Tüm filozoflar için ve ölüm olmadığı sürece
		{
			pthread_mutex_lock(&(r->t_last_meal_check)); // Son yemek zamanı kontrol mutex'ini kilitler
			if (time_diff(p[i].t_last_meal, timestamp()) > r->time_death) // Eğer bir filozofun son yemek zamanı belirtilen ölüm süresinden fazla ise
			{
				pthread_mutex_lock(&(r->died_check)); // Ölüm kontrol mutex'ini kilitler
				printf("%i ", (int)(timestamp() - r->first_timestamp)); // Zaman farkını hesaplar ve yazdırır
				printf("%i ", i + 1); // Filozofun ID'sini yazdırır
				printf("%s\n", "died"); // "died" mesajını yazdırır
				r->died = 1; // Ölüm bayrağını işaretler
				pthread_mutex_unlock(&(r->died_check)); // Ölüm kontrol mutex'ini açar
				j = -1;
				while (++j < r->nb_philo)
					pthread_mutex_unlock(&(r->forks[j])); // Tüm çatal mutex'lerini açar
			}
			pthread_mutex_unlock(&(r->t_last_meal_check)); // Son yemek zamanı kontrol mutex'ini açar
			usleep(50); // Kısa süre uyur
		}
		if (r->died) // Eğer bir filozof öldüyse
			break; // Döngüden çıkar
		eat_control(r, p); // Yemek yeme kontrolünü sağlar
	}
}

int launcher(t_rules *rules)
{
	int i;
	t_philosopher *phi;

	i = 0;
	phi = rules->philosophers; // Kurallar yapısına göre filozofları işaret eder
	while (i < rules->nb_philo) // Tüm filozoflar için
	{
		if (pthread_create(&(phi[i].thread_id), NULL, routine, &(phi[i]))) // Thread oluşturur
			exit_launcher(rules, phi); // Oluşturma başarısız olursa çıkış işlemi yapar
		pthread_mutex_lock(&(rules->t_last_meal_check)); // Son yemek zamanını kontrol mutex'ini kilitler
		phi[i].t_last_meal = timestamp(); // Son yemek zamanını günceller
		pthread_mutex_unlock(&(rules->t_last_meal_check)); // Son yemek zamanını kontrol mutex'ini açar
		i++; // Bir sonraki filozofa geçer
	}
	death_checker(rules, rules->philosophers, -1, -1); // Ölüm kontrolünü yapar
	exit_launcher(rules, phi); // Çıkış işlemi yapar
	return (0); // Başarıyla tamamlandıysa 0 döner
}
