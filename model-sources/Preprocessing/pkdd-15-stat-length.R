library(data.table)
library(e1071)
library(geosphere)
library(stringi)

trajectory_to_points <- function (s) {
  s <- unlist(stri_split_fixed(s, "]", omit_empty = TRUE))
  s <- sapply(s, function(z) stri_sub(z, 3L, nchar(z)), USE.NAMES = FALSE)
  P <- do.call(rbind, lapply(stri_split_fixed(s, ","), as.numeric)) # P is the matrix of trajectory points
  P <- as.data.table(P)
  names(P) <- c("long", "lat")
  P
}

trajectory_length <- function(s) {
  P <- trajectory_to_points(s)
  P[, `:=`(long2 = shift(long, type = "lead"), lat2 = shift(lat, type = "lead"))]
  sum(head(distGeo(P[, 1:2], P[, 3:4]), -1))
}

X <- fread("train.csv.2", colClasses = list(character = c("TRIP_ID", "POLYLINE"), integer = c("TIMESTAMP", "POINTS")))
X[, LENGTH := Vectorize(trajectory_length)(POLYLINE)]

cat("Mean:", mean(X$LENGTH), "\n")
cat("Median:", median(X$LENGTH), "\n")

Mode <- function(x) {
  ux <- unique(x)
  ux[which.max(tabulate(match(x, ux)))]
}
cat("Mode:", Mode(X$LENGTH), "\n")

cat("Standard deviation:", sd(X$LENGTH), "\n")
cat("Kurtosis:", kurtosis(X$LENGTH), "\n")
cat("Skewness:", skewness(X$LENGTH), "\n")
cat("Minimum:", min(X$LENGTH), "\n")
cat("Maximum:", max(X$LENGTH), "\n")
cat("Number of trajectories:", nrow(X), "\n")

