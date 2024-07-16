#include "philo.h" // Projenin gerekli fonksiyon ve yapı tanımlarını içeren header dosyasını dahil eder

int	main(int argc, char **argv)
{
	t_rules	rules; // Programın kurallarını ve durumlarını tutacak yapı

	if (!(argc == 5 || argc == 6)) // Argüman sayısını kontrol eder, 5 veya 6 argüman kabul eder
		return (write_error("Invalid Argument"), 0); // Argüman sayısı geçersizse hata mesajı yazdırır ve programı sonlandırır
	if (parse(&rules, argv)) // Argümanları ayrıştırarak kuralları yapılandırır
		return (write_error("Mutex Start Error"), 0); // Eğer parse başarısız olursa hata mesajı yazdırır ve programı sonlandırır
	if (init_philosophers(&rules)) // Filozofları başlatır ve gerekli ayarları yapar
		return (write_error("Philosophers Init Error"), 0); // Eğer init_philosophers başarısız olursa hata mesajı yazdırır ve programı sonlandırır
	if (init_mutex(&rules)) // Mutexleri başlatır ve gerekli ayarları yapar
		return (write_error("Mutex Init Error"), 0); // Eğer init_mutex başarısız olursa hata mesajı yazdırır ve programı sonlandırır
	if (launcher(&rules)) // Filozof iş parçacıklarını oluşturur ve başlatır
		return (write_error("Thread Create Error"), 0); // Eğer launcher başarısız olursa hata mesajı yazdırır ve programı sonlandırır
}
