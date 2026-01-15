# Çok Çekirdekli İşlemcilerde Yük Dağılımı Optimizasyonu

[cite_start]Bu çalışma, Gazi Üniversitesi Teknoloji Fakültesi Bilgisayar Mühendisliği Bölümü BMT 317 Algoritmalar dersi dönemi sonu projesi olarak geliştirilmiştir[cite: 6, 7, 8, 9, 10]. [cite_start]Proje, çok çekirdekli sistemlerde (Multi-core) iş parçacıklarının (thread) çekirdekler arasında verimli şekilde dağıtılmasını hedefleyen farklı zamanlama stratejilerinin simülasyonunu ve analizini içerir[cite: 16, 23].

## Proje Kapsamı ve Uygulama

[cite_start]Sistem üzerinde 8 çekirdek ve 500 thread kullanılarak [cite: 77] aşağıdaki 5 farklı algoritmanın performansı karşılaştırılmıştır:

* [cite_start]**Round-Robin (RR):** Thread'leri yük durumuna bakmaksızın sırayla çekirdeklere atar[cite: 33, 61].
* [cite_start]**Least-Loaded:** İş yükü miktarını takip ederek her yeni thread'i o an en az yüke sahip çekirdeğe yönlendirir[cite: 37, 62].
* [cite_start]**Priority-Based:** Thread önceliklerini dikkate alarak yük dengeleme yapar; yüksek öncelikli işlere atama önceliği tanır[cite: 40, 63].
* [cite_start]**Affinity-Based:** Cache verimliliği için thread'leri belirli çekirdek gruplarında tutmaya çalışır, ancak yük farkı eşiği aşıldığında diğer çekirdeklere kaydırır[cite: 44, 64].
* [cite_start]**Work-Stealing:** Dinamik bir yaklaşım olarak, yoğun çekirdeklerden yükü alıp boş çekirdeklere dağıtarak varyansı minimize eder[cite: 47, 65].

## Teknik Detaylar ve Analiz Metrikleri

Algoritmaların başarısını ölçmek için kullanılan temel metrikler şunlardır:
- [cite_start]**Yük Varyansı (Maliyet):** Çekirdekler arasındaki yük dağılımının dengesizliğini temsil eder[cite: 74, 113].
- [cite_start]**Hız (Speed):** En yoğun çekirdeğin yüküne ters orantılı olarak hesaplanan (1/MaxLoad) değerdir[cite: 113].
- [cite_start]**Verimlilik:** Hızın maliyete (varyansa) bölünmesiyle elde edilen genel başarı oranıdır[cite: 112, 113].

## Zaman Karmaşıklığı (Big O) Analizi

Simülasyon kapsamında kullanılan yaklaşımların teorik karmaşıklıkları şu şekildedir:
- [cite_start]**Round-Robin:** $O(n)$ [cite: 115]
- [cite_start]**Least-Loaded:** $O(n \cdot k)$ [cite: 116]
- [cite_start]**Priority-Based:** $O(n \cdot k)$ veya $O(n \log n)$ [cite: 117]
- [cite_start]**Affinity-Based:** $O(n)$ + kontrol adımı [cite: 118]
- [cite_start]**Work-Stealing:** $O(n \cdot k)$ + dengeleme maliyeti [cite: 119]

*(n: Thread sayısı, k: Çekirdek sayısı)*

## Deneysel Sonuçlar

Yapılan testlerde elde edilen karşılaştırma tablosu:

| Algoritma | Maks. Çekirdek Yükü | Yük Varyansı | Verimlilik (x10^6) |
| :--- | :---: | :---: | :---: |
| Round-Robin | 661 | 993.94 | 1.522 |
| Least-Loaded | 627 | 23.94 | 66.628 |
| Priority-Based | 630 | 33.94 | 46.771 |
| Affinity-Based | 683 | 2208.94 | 0.663 |
| Work-Stealing | **623** | **4.44** | **361.721** |

[cite_start]Analiz sonuçlarına göre Work-Stealing algoritması, yük varyansını en düşük seviyede tutarak en yüksek verimlilik puanına ulaşmıştır[cite: 88, 89, 96].

## Kurulum ve Derleme

[cite_start]Proje C dili ile geliştirilmiştir[cite: 57]. Derleme için CMake kullanılabilir:

```bash
mkdir build
cd build
cmake ..
make
./core_thread_relation