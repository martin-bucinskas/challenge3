import json
import math
import sys
import timeit
import csv
from City import City


# This wont really work...
# The way I need to do it is:
# 1. Take a starting city
# 2. Calculate the ma


def get_km_between_coords(lat1, long1, lat2, long2):
    phi1 = math.radians(lat1)
    phi2 = math.radians(lat2)
    delta_lambda = math.radians(long2 - long1)
    r = 6371e3
    d = math.acos(math.sin(phi1) * math.sin(phi2) + math.cos(phi1) * math.cos(phi2) * math.cos(delta_lambda)) * r
    return d / 1000


def load_json(path):
    with open(path, 'r') as f:
        data = json.load(f)
        return data


def load_cities(json_data):
    cities = list()
    for city in json_data:
        if city['capitalCity'] is not '':
            city_obj = City(city['name'], city['capitalCity'], city['latitude'], city['longitude'])

            cities.append(city_obj)

    return cities


def get_nearest_non_visited_city(city_obj, city_list):
    distance_list = list()

    for city in city_list:
        if city.get_capital() is not city_obj.get_capital() and city not in visited_cities:
            entry = {
                'city': city,
                'distance': get_km_between_coords(city_obj.get_latitude(), city_obj.get_longitude(),
                                                  city.get_latitude(), city.get_longitude())
            }
            distance_list.append(entry)
    ordered_cities = sorted(distance_list, key=lambda k: k['distance'])

    for city_entry in ordered_cities:
        if city_entry['city'].is_visited() is False:
            return city_entry


visited_cities = list()


def calc(starting_city_obj, city_list, max_distance):
    distance_travelled = max_distance
    nearest_city = get_nearest_non_visited_city(starting_city_obj, city_list)
    # Check if exceeded the distance
    if nearest_city['distance'] <= max_distance:
        print(distance_travelled)
        visited_cities.append(nearest_city['city'])
        distance_travelled -= nearest_city['distance']
        calc(nearest_city['city'], city_list, distance_travelled)


def main():
    data = load_json('data/sexy_countries.json')
    cities = load_cities(data)
    starting_city = None

    param = sys.argv[1]
    details = param.split('|')

    for city in cities:
        if city.get_country() == details[1]:
            starting_city = city

    visited_cities.append(starting_city)
    calc(starting_city, cities, float(details[0]))

    print(len(visited_cities), end=' ')
    count = 0
    for city in visited_cities:
        print(city.get_capital(), end='')
        count += 1
        if count < len(visited_cities):
            print(',', end='')
    print()


def convert_json_to_csv(json_obj):
    f = csv.writer(open('sexy_countries.csv', 'w+'))
    f.writerow(['country', 'capital', 'longitude', 'latitude'])

    for country in json_obj:
        if country['capitalCity'] is not '':
            f.writerow([country['name'], country['capitalCity'], country['longitude'], country['latitude']])


if __name__ == '__main__':
    data = load_json('data/sexy_countries.json')
    convert_json_to_csv(data)
    # time_taken = timeit.timeit(main, number=1)
    # print(time_taken)
    # print(str(time_taken * 1000) + 'ms')
