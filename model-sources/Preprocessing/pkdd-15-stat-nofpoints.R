library(data.table)
library(e1071)

X <- fread("train.csv.2", colClasses = list(character = c("TRIP_ID", "POLYLINE"), integer = c("TIMESTAMP", "POINTS")))

cat("Mean:", mean(X$POINTS), "\n")
cat("Median:", median(X$POINTS), "\n")

Mode <- function(x) {
  ux <- unique(x)
  ux[which.max(tabulate(match(x, ux)))]
}
cat("Mode:", Mode(X$POINTS), "\n")

cat("Standard deviation:", sd(X$POINTS), "\n")
cat("Kurtosis:", kurtosis(X$POINTS), "\n")
cat("Skewness:", skewness(X$POINTS), "\n")
cat("Minimum:", min(X$POINTS), "\n")
cat("Maximum:", max(X$POINTS), "\n")
cat("Number of trajectories:", nrow(X), "\n")

