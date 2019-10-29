import json
import math
import sys
import timeit
import csv
import itertools
import multiprocessing as mp
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
    index_lookup_json_list = dict()
    c = 0
    cities = list()
    for city in json_data:
        if city['capitalCity'] is not '':
            ilj = {
                city['name']: {
                    'index': c + 4,
                    'capital_city': city['capitalCity']
                }
            }
            index_lookup_json_list.update(ilj)
            city_obj = City(city['name'], city['capitalCity'], city['latitude'], city['longitude'], c + 4)
            c += 1
            cities.append(city_obj)

    with open('lookup_index.json', 'w') as outfile:
        json.dump(index_lookup_json_list, outfile, indent=4)

    print('Loaded %s countries.' % c)
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


def get_possible_countries(row, max_distance, table):
    possible_countries = list()

    count = 4
    while count <= 214:
        distance = float(row[count])
        if max_distance >= distance > -1:
            print(distance)
            possible_countries.append(table[0][count])
        count += 1
    return possible_countries


lookup_index = dict()


def get_lookup_table_details(country_name):
    return lookup_index.get(country_name)['index']


def get_lookup_table_capitals(country_name):
    return lookup_index.get(country_name)['capital_city']


def calc_distance_taken(queue_of_countries, starting_country, max_distance, table):
    distance = 0
    country1 = starting_country
    for country2 in queue_of_countries:
        # print('Country1: %s\nCountry2: %s' % (country1, country2))
        distance += float(table[get_lookup_table_details(country1) - 3][get_lookup_table_details(country2)])
        # print('Distance: %s' % distance)
        country1 = country2
        if distance > max_distance:
            return -1

    # print('Total Distance: %s' % distance)
    return distance


def parallel_calc_subsets(subset_length, starting_country, max_distance, table, possible_countries):
    print(subset_length)
    possible_queue = None
    for subset in itertools.combinations(possible_countries, subset_length):
        distance_taken = calc_distance_taken(subset, starting_country, max_distance, table)
        if distance_taken > -1:
            possible_queue = subset
            break
    if possible_queue is None:
        return []
    return possible_queue


results = []


def collect_result(result):
    global results
    # print(result)
    results.append(result)


def calc_all_routes(starting_country, possible_countries, table, max_distance):
    # distance_remaining = max_distance
    possible_queue = list()
    max_possibilities = len(possible_countries) + 1

    pool = mp.Pool(mp.cpu_count())

    results = pool.starmap_async(parallel_calc_subsets, [(L, starting_country, max_distance, table, possible_countries) for L in
                                               reversed(range(0, len(possible_countries) + 1))]).get()

    # THIS WORKS!
    # for L in reversed(range(0, len(possible_countries) + 1)):
    #     pool.apply_async(parallel_calc_subsets, args=(L, starting_country, max_distance, table, possible_countries), callback=collect_result)

    # my_results = [
    #     pool.apply_async(parallel_calc_subsets, args=(L, starting_country, max_distance, table, possible_countries)) for L in reversed(range(0, len(possible_countries) + 1))
    # ]

    # Check the results to see if it contains anything with the longest. (to prevent from continuing to check everything!!!!!!!!!!!!!!!!

    pool.close()
    pool.join()

    # print(results)

    # print(results)
    # results.sort(key=len)
    # print(results)
    longest = None
    last_length = 0

    for result in results:
        # print(len(result))
        if len(result) > last_length:
            longest = result
            last_length = len(result)

    size = len(longest) + 1
    print('%s %s,' % (size, get_lookup_table_capitals(starting_country)), end='')
    count = 1
    end = ''

    for country in longest:
        count += 1
        if count >= size:
            end = '\n'
        else:
            end = ','
        print('%s' % (get_lookup_table_capitals(country)), end=end)

    # for L in reversed(range(0, len(possible_countries) + 1)):
    #
    #     print(L)
    #     for subset in itertools.combinations(possible_countries, L):
    #         # print(subset)
    #         # print('Subset Length: ' % (len(subset), ))
    #         # print('_' * 100)
    #         # print(subset)
    #         distance_taken = calc_distance_taken(subset, starting_country, max_distance, table)
    #         if distance_taken > -1:
    #             # print('Distance Taken: %s' % distance_taken)
    #             possible_queue.append(subset)
    #             break
    #     else:
    #         continue
    #     break

    # possible = None
    # possible_len = 0
    # for p in possible_queue:
    #     if len(p) > possible_len:
    #         possible = p
    #         possible_len = len(p)
    # print('Found:')
    # print(possible)
    # size = len(possible) + 1
    # print('%s %s,' % (size, get_lookup_table_capitals(starting_country)), end='')
    # count = 1
    # end = ''
    #
    # for country in possible:
    #     count += 1
    #     if count >= size:
    #         end = '\n'
    #     else:
    #         end = ','
    #     print('%s' % (get_lookup_table_capitals(country)), end=end)
    # print(possible_queue)
    # for country in possible_countries:
    #     distance_remaining -= table[starting_country.get_position() - 3][country.get_position()]
    #


def calc_main():
    data = load_json('data/sexy_countries.json')
    cities = load_cities(data)
    starting_city = None

    param = sys.argv[1]
    details = param.split('|')

    for city in cities:
        if city.get_country() == details[1]:
            starting_city = city

    already_visited_cities = list()

    already_visited_cities.append(starting_city)

    csv_data = list(csv.reader(open('sexy_countries.csv')))

    row = csv_data[starting_city.get_position() - 3]

    possible_countries = get_possible_countries(row, float(details[0]), csv_data)
    print(possible_countries)
    print('Possible %s countries' % len(possible_countries))

    calc_all_routes(starting_city.get_country(), possible_countries, csv_data, float(details[0]))


def convert_json_to_csv(json_obj):
    f = csv.writer(open('sexy_countries.csv', 'w+'))
    f.writerow(['country', 'capital', 'longitude', 'latitude'])

    for country in json_obj:
        if country['capitalCity'] is not '':
            f.writerow([country['name'], country['capitalCity'], country['longitude'], country['latitude']])


def calc_distance_for_everything(json_obj):
    headers = ['country', 'capital', 'longitude', 'latitude']
    for country in json_obj:
        if country['capitalCity'] is not '':
            headers.append(country['name'])

    f = csv.writer(open('sexy_countries.csv', 'w+'))
    f.writerow(headers)
    country_distances = list()

    distance = 0
    row = None
    for country1 in json_obj:
        if country1['capitalCity'] is not '':
            print('Country: %s' % (country1['name']))
            for country2 in json_obj:
                if country2['capitalCity'] is not '':
                    if country1['name'] is not country2['name']:
                        distance = get_km_between_coords(float(country1['latitude']), float(country1['longitude']),
                                                         float(country2['latitude']), float(country2['longitude']))
                    else:
                        distance = -1
                    country_distances.append(distance)
            row = [country1['name'], country1['capitalCity'], country1['longitude'],
                   country1['latitude']]
            row.extend(country_distances)
            f.writerow(row)
            country_distances = list()


# def get_list_of_countries_within_distance(starting_country, row, max_distance):
#     countries = list()
#
#     # countries start at 4 and end at 211+4
#     count = 4
#     while count <= 211 + 4:
#         if count is not starting_country.get_position():
#             if row[count] <= max_distance:
#                 countries.append(row[count])

# def create_create_query(json_obj):
# #     CREATE (:Country {CountryName:countries[0], Capital:countries[1], Longitude:countries[2], Latitude:countries[3]})
#     query = ['CREATE', '(:Country{ CountryName:countries[0], Capital:countries[1], Longitude:countries[2], Latitude:countries[3], ']
#
#     q = None
#
#     count = 4
#     for country in json_obj:
#         if country['capitalCity'] is not '':
#             q = country['name'].replace(' ', '_').replace('.', '_').replace('\'', '')\
#                     .replace('’', '').replace(',', '').replace('-', '_')\
#                     .replace('(', '').replace(')', '') + ':countries[' + str(count) + '], '
#             count += 1
#             query.extend(q)
#
#     query.append('})')
#
#     print(''.join(query))


# MATCH (n:rec_sku)
#  WITH collect(n) as plist
#  FOREACH (x IN plist |
#  FOREACH (y IN filter(z in plist WHERE x.also_bought_sku = z.also_bought_sku)      |
#  CREATE (y)-[:SAME_SKU]->(x)
#  )
#  );
# def create_match_query(json_obj):
#     query = []
#
#     for country in json_obj:
#         if country['capitalCity'] is not '':
#             query.append('MATCH (n:Country)\nWITH collect(n) as plist\nFOREACH (x IN plist |\nFOREACH (y IN filter(z in plist WHERE x.')
#             country_name = country['name'].replace(' ', '_').replace('.', '_').replace('\'', '') \
#                 .replace('’', '').replace(',', '').replace('-', '_') \
#                 .replace('(', '').replace(')', '')
#             query.append(country_name)
#             query.append(' = z.')
#             query.append(country_name)
#             query.append(') |\nCREATE (y)-[r:DISTANCE]->(x)\n)\n);\n\n')
#             print(''.join(query))
#             file = open('create_relationships_updated', 'w+')
#             file.write(''.join(query))


if __name__ == '__main__':
    # data = load_json('data/sexy_countries.json')
    # create_create_query(data)
    # create_match_query(data)
    # calc_distance_for_everything(data)

    with open('lookup_index.json', 'rb') as f:
        lookup_index = json.load(f)

    # print(lookup_index.get('Aruba'))

    time_taken = timeit.timeit(calc_main, number=1)
    # print(time_taken)
    print(str(time_taken * 1000) + 'ms')
