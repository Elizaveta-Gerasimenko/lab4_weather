#include <gtest/gtest.h>
#include "library.hpp"

#include <gtest/gtest.h>

TEST(FacadeAdapterTest, InvalidXmlData) {
    class BrokenXmlService : public XmlWeatherService {
    public:
        std::string getWeatherXml() const override {
            return "<weather><temp>25</temp></weather>"; // Нет тега <temperature>
        }
    };

    BrokenXmlService brokenService;
    XmlToJsonAdapter adapter(brokenService);

    // Ожидаем исключение или возврат значения по умолчанию
    EXPECT_THROW(adapter.getTemperature(), std::invalid_argument);
}

TEST(FacadeAdapterTest, EmptyJsonData) {
    class EmptyJsonService : public JsonWeatherService {
    public:
        std::string getWeatherJson() const override {
            return "{}"; // Пустой JSON
        }
    };

    EmptyJsonService emptyService;
    JsonWeatherServiceImpl serviceImpl(emptyService);

    // Ожидаем обработку ошибки
    ASSERT_EQ(serviceImpl.getTemperature(), -1); // Или исключение
}

TEST(FacadeAdapterTest, MultipleAdapters) {
    // Сервисы
    XmlWeatherService xmlService;
    JsonWeatherService jsonService;

    // Адаптеры
    auto xmlAdapter = std::make_unique<XmlToJsonAdapter>(xmlService);
    auto jsonAdapter = std::make_unique<JsonWeatherServiceImpl>(jsonService);

    // Фасад с двумя адаптерами
    WeatherFacade facade(std::move(jsonAdapter), std::move(xmlAdapter));
    
    ASSERT_EQ(facade.getJsonTemperature(), 18);
    ASSERT_EQ(facade.getXmlTemperature(), 25);
}

TEST(FacadeAdapterTest, NegativeTemperature) {
    class NegativeXmlService : public XmlWeatherService {
    public:
        std::string getWeatherXml() const override {
            return "<weather><temperature>-10</temperature></weather>";
        }
    };

    NegativeXmlService service;
    XmlToJsonAdapter adapter(service);
    
    ASSERT_EQ(adapter.getTemperature(), -10);
}

TEST(FacadeAdapterTest, NonStandardJson) {
    class CustomJsonService : public JsonWeatherService {
    public:
        std::string getWeatherJson() const override {
            return "{ \"temp\": 35 }"; // Ключ "temp" вместо "temperature"
        }
    };

    CustomJsonService customService;
    JsonWeatherServiceImpl serviceImpl(customService);

    // Ожидаем ошибку парсинга
    ASSERT_EQ(serviceImpl.getTemperature(), -1);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
