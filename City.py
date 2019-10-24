class City:

    def __init__(self, country, capital, latitude, longitude):
        self.visited = False
        self.country = country
        self.capital = capital
        self.latitude = float(latitude)
        self.longitude = float(longitude)

    def get_country(self) -> str:
        return self.country

    def get_capital(self) -> str:
        return self.capital

    def get_latitude(self) -> float:
        return self.latitude

    def get_longitude(self) -> float:
        return self.longitude

    def is_visited(self) -> bool:
        return self.visited

    def set_visited(self, visited) -> None:
        self.visited = visited
        return None
