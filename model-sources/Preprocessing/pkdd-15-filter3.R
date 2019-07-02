library(data.table)
library(lubridate)

X <- fread("train.csv.2", colClasses = list(character = c("TRIP_ID", "POLYLINE"), integer = c("TIMESTAMP", "POINTS")))

X[, TS := as_datetime(TIMESTAMP)]

Y <- X[, .(trip_id = 1:.N, timestamp = TIMESTAMP, year = year(TS), month = month(TS), day = day(TS), hour = hour(TS), minute = minute(TS), second = second(TS), point = POINTS, polyline = POLYLINE)]

fwrite(Y, "pkdd-15-subset-all.csv", quote = TRUE)

train <- sort(sample(1:nrow(Y), 0.6 * nrow(Y)))

fwrite(Y[train], "pkdd-15-subset-train.csv", quote = TRUE)
fwrite(Y[-train], "pkdd-15-subset-test.csv", quote = TRUE)

