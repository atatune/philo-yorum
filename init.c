#include "philo.h" // Projenin gerekli fonksiyon ve yapı tanımlarını içeren header dosyasını dahil eder
#include <stdlib.h> // Bellek yönetimi ve diğer yardımcı fonksiyonlar için stdlib kütüphanesini dahil eder

int	init_mutex(t_rules *rules)
{
	int	i; // Sayaç değişkeni

	i = rules->nb_philo; // Filozof sayısını i'ye atar
	rules->forks = malloc(sizeof(pthread_mutex_t) * i); // Çatalları temsil eden mutexleri tutmak için bellek tahsis eder
	if (!(rules->forks)) // Bellek tahsisi başarısız olursa
		return (free(rules->philosophers), 1); // Filozoflar için ayrılan belleği serbest bırakır ve hata döner
	if (!destroy_mutex(rules)) // Mutexlerin başlatılmasını kontrol eder
		return (free(rules->philosophers), 1); // Başarısız olursa filozoflar için ayrılan belleği serbest bırakır ve hata döner
	while (--i >= 0) // Her çatal için mutex başlatır
	{
		if (pthread_mutex_init(&(rules->forks[i]), NULL)) // Mutex başlatma başarısız olursa
		{
			while (++i < rules->nb_philo) // Başarıyla başlatılan mutexleri yok eder
				pthread_mutex_destroy(&(rules->forks[i])); 
			free(rules->philosophers); // Filozoflar için ayrılan belleği serbest bırakır
			return (chose_mutex(rules, 6), 1); // Başlatma hatası durumunda mutexleri yok eder ve hata döner
		}
	}
	return (0); // Başarıyla tamamlandıysa 0 döner
}

int	destroy_mutex(t_rules *rules)
{
	if (pthread_mutex_init(&(rules->meal_check), NULL)) // meal_check mutexini başlatır
		return (0); // Başarısız olursa 0 döner
	else if (pthread_mutex_init(&(rules->die_check), NULL)) // die_check mutexini başlatır
		return (chose_mutex(rules, 1), 0); // Başarısız olursa diğer mutexleri yok eder ve 0 döner
	else if (pthread_mutex_init(&(rules->x_ate_check), NULL)) // x_ate_check mutexini başlatır
		return (chose_mutex(rules, 2), 0); // Başarısız olursa diğer mutexleri yok eder ve 0 döner
	else if (pthread_mutex_init(&(rules->t_last_meal_check), NULL)) // t_last_meal_check mutexini başlatır
		return (chose_mutex(rules, 3), 0); // Başarısız olursa diğer mutexleri yok eder ve 0 döner
	else if (pthread_mutex_init(&(rules->died_check), NULL)) // died_check mutexini başlatır
		return (chose_mutex(rules, 4), 0); // Başarısız olursa diğer mutexleri yok eder ve 0 döner
	else if (pthread_mutex_init(&(rules->all_ate_check), NULL)) // all_ate_check mutexini başlatır
		return (chose_mutex(rules, 5), 0); // Başarısız olursa diğer mutexleri yok eder ve 0 döner
	return (1); // Başarıyla tamamlandıysa 1 döner
}

void	chose_mutex(t_rules *rules, int x)
{
	if (x >= 1)
		pthread_mutex_destroy(&(rules->meal_check)); // meal_check mutexini yok eder
	if (x >= 2)
		pthread_mutex_destroy(&(rules->die_check)); // die_check mutexini yok eder
	if (x >= 3)
		pthread_mutex_destroy(&(rules->x_ate_check)); // x_ate_check mutexini yok eder
	if (x >= 4)
		pthread_mutex_destroy(&(rules->t_last_meal_check)); // t_last_meal_check mutexini yok eder
	if (x >= 5)
		pthread_mutex_destroy(&(rules->died_check)); // died_check mutexini yok eder
	if (x >= 6)
		pthread_mutex_destroy(&(rules->all_ate_check)); // all_ate_check mutexini yok eder
}

int	init_philosophers(t_rules *rules)
{
	int	i; // Sayaç değişkeni

	i = rules->nb_philo; // Filozof sayısını i'ye atar
	rules->philosophers = malloc(sizeof(t_philosopher) * i); // Filozofları temsil eden yapı için bellek tahsis eder
	if (!(rules->philosophers)) // Bellek tahsisi başarısız olursa
		return (1); // Hata döner
	i = -1;
	while (++i < rules->nb_philo) // Her filozof için yapılandırma yapar
	{
		rules->philosophers[i].id = i + 1; // Filozofun ID'sini atar
		rules->philosophers[i].x_ate = 0; // Filozofun yediği yemek sayısını sıfırlar
		rules->philosophers[i].left_fork_id = i; // Filozofun solundaki çatalın ID'sini atar
		if (!i)
			rules->philosophers[i].right_fork_id = rules->nb_philo - 1; // İlk filozof için sağdaki çatalın ID'sini son çatal olarak atar
		else
			rules->philosophers[i].right_fork_id = i - 1; // Diğer filozoflar için sağdaki çatalın ID'sini önceki çatal olarak atar
		rules->philosophers[i].t_last_meal = 0; // Filozofun son yemeğini yediği zamanı sıfırlar
		rules->philosophers[i].rules = rules; // Filozofun kurallar yapısını atar
	}
	return (0); // Başarıyla tamamlandıysa 0 döner
}

int	parse(t_rules *rules, char **argv)
{
	rules->nb_philo = ft_atoi(argv[1]); // Argümanlardan filozof sayısını alır
	rules->time_death = ft_atoi(argv[2]); // Argümanlardan ölüm süresini alır
	rules->time_eat = ft_atoi(argv[3]); // Argümanlardan yeme süresini alır
	rules->time_sleep = ft_atoi(argv[4]); // Argümanlardan uyuma süresini alır
	rules->first_timestamp = timestamp(); // Programın başlangıç zamanını alır
	rules->all_ate = 0; // Bütün filozofların yemek yeme durumu başlangıçta sıfırlanır
	rules->died = 0; // Filozoflardan herhangi birinin ölme durumu başlangıçta sıfırlanır
	if (rules->nb_philo <= 0 || (int)rules->time_death <= 0
		|| rules->time_eat <= 0 || rules->time_sleep <= 0) // Argümanların geçerliliğini kontrol eder
		return (1); // Geçersiz argüman varsa hata döner
	if (argv[5]) // Opsiyonel beşinci argüman (yemek yeme sayısı) varsa
	{
		rules->nb_eat = ft_atoi(argv[5]); // Argümandan yemek yeme sayısını alır
		if (rules->nb_eat <= 0) // Yemek yeme sayısı geçersizse
			return (1); // Hata döner
	}
	else
		rules->nb_eat = -1; // Beşinci argüman yoksa yemek yeme sayısını sınırsız olarak ayarlar
	return (0); // Başarıyla tamamlandıysa 0 döner
}