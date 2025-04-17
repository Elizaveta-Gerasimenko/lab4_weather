#pragma once
#include <deque>
#include <vector>
#include <string>
#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>

// Интерфейс для сервиса погоды
class IWeatherService {
public:
    virtual ~IWeatherService() = default;
    virtual int getTemperature() const = 0;
};

// Интерфейс для адаптера
class IWeatherAdapter {
public:
    virtual ~IWeatherAdapter() = default;
    virtual int getConvertedTemperature() const = 0;
};

// Внешние сервисы
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

// Адаптер для XmlWeatherService
class XmlToJsonAdapter : public IWeatherService {
public:
    XmlToJsonAdapter(const XmlWeatherService& service) : xmlService(service) {}

    int getTemperature() const override {
        std::string xml = xmlService.getWeatherXml();
        size_t start = xml.find("<temperature>");
        if (start == std::string::npos) {
            throw std::invalid_argument("Invalid XML format: temperature tag not found");
        }
        start += 13; // length of "<temperature>"
        size_t end = xml.find("</temperature>", start);
        if (end == std::string::npos) {
            throw std::invalid_argument("Invalid XML format: closing temperature tag not found");
        }

        std::string tempStr = xml.substr(start, end - start);
        try {
            return std::stoi(tempStr);
        }
        catch (const std::exception&) {
            throw std::invalid_argument("Invalid temperature value in XML");
        }
    }

private:
    const XmlWeatherService& xmlService;
};

// Реализация JsonWeatherService как IWeatherService
class JsonWeatherServiceImpl : public IWeatherService {
public:
    JsonWeatherServiceImpl(const JsonWeatherService& service) : jsonService(service) {}

    int getTemperature() const override {
        std::string json = jsonService.getWeatherJson();
        size_t start = json.find("\"temperature\":");
        if (start == std::string::npos) {
            return -1; // или можно бросить исключение
        }
        start += 14; // length of "\"temperature\":"
        size_t end = json.find_first_of(",}", start);
        if (end == std::string::npos) {
            return -1;
        }

        std::string tempStr = json.substr(start, end - start);
        try {
            return std::stoi(tempStr);
        }
        catch (const std::exception&) {
            return -1;
        }
    }

private:
    const JsonWeatherService& jsonService;
};

// Фасад
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