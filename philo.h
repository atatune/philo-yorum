#ifndef PHILO_H
# define PHILO_H // Header dosyasının birden fazla kez dahil edilmesini önler

# include <pthread.h> // pthread kütüphanesini dahil eder

typedef struct s_philosopher
{
	u_int64_t		id; // Filozofun ID'si
	int				x_ate; // Filozofun yediği yemek sayısı
	int				left_fork_id; // Filozofun solundaki çatalın ID'si
	int				right_fork_id; // Filozofun sağındaki çatalın ID'si
	u_int64_t		t_last_meal; // Filozofun son yemek yediği zaman
	struct s_rules	*rules; // Kurallar yapısına işaretçi
	pthread_t		thread_id; // Filozofun thread ID'si
}					t_philosopher;

typedef struct s_rules
{
	int				nb_philo; // Filozof sayısı
	u_int64_t		time_death; // Ölüm süresi
	int				time_eat; // Yemek yeme süresi
	int				time_sleep; // Uyuma süresi
	int				nb_eat; // Yemek yeme sayısı
	int				all_ate; // Tüm filozoflar yedi mi
	int				died; // Bir filozof öldü mü
	u_int64_t		first_timestamp; // İlk zaman damgası
	pthread_mutex_t	t_last_meal_check; // Son yemek yeme kontrol mutexi
	pthread_mutex_t	x_ate_check; // Yeme kontrol mutexi
	pthread_mutex_t	die_check; // Ölüm kontrol mutexi
	pthread_mutex_t	died_check; // Ölüm kontrol mutexi
	pthread_mutex_t	all_ate_check; // Tüm yeme kontrol mutexi
	pthread_mutex_t	meal_check; // Yemek kontrol mutexi
	pthread_mutex_t	*forks; // Çatal mutexleri
	t_philosopher	*philosophers; // Filozoflar dizisi
}					t_rules;

/*
** ----- utils2.c -----
*/

int					write_error(char *str); // Hata mesajı yazdırır
void				eat_control(t_rules *r, t_philosopher *p); // Yemek yeme kontrolünü sağlar
void				ft_free(t_rules *rules); // Belleği serbest bırakır
void				action_print(t_rules *rules, int id, char *string); // Filozofun eylemini yazdırır
void				philo_eats(t_philosopher *philo); // Filozofun yemek yemesini sağlar

/*
** ----- init.c -----
*/

int					parse(t_rules *rules, char **argv); // Kuralları parse eder
int					init_philosophers(t_rules *rules); // Filozofları başlatır
int					init_mutex(t_rules *rules); // Mutexleri başlatır

/*
** ----- utils.c -----
*/

int					ft_atoi(const char *str); // String'i integer'a çevirir
u_int64_t			timestamp(void); // Zaman damgası alır
u_int64_t			time_diff(u_int64_t past, u_int64_t pres); // Zaman farkını hesaplar
void				smart_sleep(u_int64_t time, t_rules *rules); // Akıllı uyuma fonksiyonu

/*
** ----- launcher.c -----
*/

int					launcher(t_rules *rules); // Ana başlatma fonksiyonu
void				exit_launcher(t_rules *rules, t_philosopher *philos); // Çıkış işlemlerini yapar

int					destroy_mutex(t_rules *rules); // Mutexleri yok eder
void				chose_mutex(t_rules *rules, int x); // Belirli bir mutex'i seçer ve yok eder
void				death_checker(t_rules *r, t_philosopher *p, int i, int j); // Ölüm kontrolünü yapar

#endif // Header dosyasının sonu
