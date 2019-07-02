library(data.table)
library(lubridate)
library(stringi)

X <- fread("train.csv", select = c("TRIP_ID", "TIMESTAMP", "MISSING_DATA", "POLYLINE"), colClasses = list(character = c("TRIP_ID", "POLYLINE"), logical = "MISSING_DATA", integer = "TIMESTAMP"))

# Removing incomplete trajectories:
incomplete <- X[MISSING_DATA == TRUE, which = TRUE]
if (length(incomplete) > 0) {
	X <- X[-incomplete]
}
cat("Incomplete trajectories removed:", length(incomplete), "\n")

# Removing the MISSING_DATA column:
X[, MISSING_DATA := NULL]

# Removing empty trajectories:
empty <- X[POLYLINE == "[]", which = TRUE]
if (length(empty) > 0) {
	X <- X[-empty]
}
cat("Empty trajectories removed:", length(empty), "\n")

# Adding the number of trajectory points:
X[, POINTS := as.integer((stri_count_fixed(POLYLINE, ",") + 1) / 2)]

# Removing single point trajectories:
single_point <- X[POINTS == 1L, which = TRUE]
if (length(single_point) > 0) {
	X <- X[-single_point]
}
cat("Single point trajectories removed:", length(single_point), "\n")

# Ordering the trajectories:
X <- X[order(TIMESTAMP)]

X[, TS := as_datetime(TIMESTAMP)]

# X <- X[wday(TS) == 2] # Keeping only trajectories starting on Mondays (1 = Sunday, 2 = Monday, ...)
X <- X[hour(TS) == 8] # Keeping only trajectories starting after 08:00 and before 09:00

# Removing the TS column:
X[, TS := NULL]

cat("Number of remaining trajectories:", nrow(X), "\n")

fwrite(X, "train.csv.1", quote = TRUE)

