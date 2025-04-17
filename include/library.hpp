#pragma once
#include <deque>
#include <vector>
#include <string>
#include <iostream>
#include <string>
#include <memory>
#include <gtest/gtest.h>

// ------------------------------
// Абстрактные интерфейсы (должны быть реализованы)
// ------------------------------

// Интерфейс для сервиса погоды
class IWeatherService {
public:
    virtual ~IWeatherService() = default;
    virtual int getTemperature() const = 0; // TODO: реализовать в наследниках
};

// Интерфейс для адаптера
class IWeatherAdapter {
public:
    virtual ~IWeatherAdapter() = default;
    virtual int getConvertedTemperature() const = 0; // TODO: реализовать
};

// ------------------------------
// Внешние сервисы (неизменяемый код)
// ------------------------------

class XmlWeatherService {
public:
    virtual std::string getWeatherXml() const {
        return "<weather><temperature>25</temperature></weather>";
    }
};

class JsonWeatherService {
public:
    virtual std::string getWeatherJson() const {
        return "{ \"temperature\": 18 }";
    }
};

// ------------------------------
// Ваш код ниже (реализуйте недостающие части)
// ------------------------------

// Адаптер: преобразует XmlWeatherService в IWeatherService
class XmlToJsonAdapter : public IWeatherService {
public:
    XmlToJsonAdapter(const XmlWeatherService& service) : xmlService(service) {}

    int getTemperature() const override {
        // TODO: распарсить XML и вернуть температуру
        return 0; 
    }

private:
    const XmlWeatherService& xmlService;
};

// Реализация JsonWeatherService как IWeatherService
class JsonWeatherServiceImpl : public IWeatherService {
public:
    JsonWeatherServiceImpl(const JsonWeatherService& service) : jsonService(service) {}

    int getTemperature() const override {
        // TODO: распарсить JSON и вернуть температуру
        return 0;
    }

private:
    const JsonWeatherService& jsonService;
};

// Фасад (должен использовать только интерфейсы!)
class WeatherFacade {
public:
    WeatherFacade(
        std::unique_ptr<IWeatherService> jsonService,
        std::unique_ptr<IWeatherService> xmlService
    ) : jsonService(std::move(jsonService)), 
        xmlService(std::move(xmlService)) {}

    int getJsonTemperature() const {
        return jsonService->getTemperature();
    }

    int getXmlTemperature() const {
        return xmlService->getTemperature();
    }

private:
    std::unique_ptr<IWeatherService> jsonService;
    std::unique_ptr<IWeatherService> xmlService;
};
