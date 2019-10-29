function gotVisits(visits) {
  console.log("Visit count: " + visits.length);
  for (visit of visits) {
    console.log(visit.visitTime);
  }
}

function listVisits(historyItems) {
  if (historyItems.length) {
    console.log("URL " + historyItems[0].url);
    var gettingVisits = browser.history.getVisits({
      url: historyItems[0].url
    });
    gettingVisits.then(gotVisits);
  }
}

var searching = browser.history.search({
  text: "",
  startTime: 0,
  maxResults: 1
});

searching.then(listVisits);