library(data.table)
library(lubridate)
library(stringi)

X <- fread("train.csv.1", colClasses = list(character = c("TRIP_ID", "POLYLINE"), integer = c("TIMESTAMP", "POINTS")))

X[, STATUS := factor("Unchanged", levels = c("Unchanged", "Deleted", "Truncated"))]

for (i in 1:nrow(X)) {
	# Breaking the value of the POLYLINE column into trajectory points:
	s <- unlist(stri_split_fixed(X[i, POLYLINE], "]", omit_empty = TRUE))
	s <- sapply(s, function(z) stri_sub(z, 3L, nchar(z)), USE.NAMES = FALSE)
	P <- do.call(rbind, lapply(stri_split_fixed(s, ","), as.numeric)) # P is the matrix of trajectory points
	P <- as.data.table(P)
	inside.bbox <- P[-8.6518 <= V1 & V1 <= -8.5771 & 41.1129 <= V2 & V2 <= 41.1756, which = TRUE]
	if (length(inside.bbox) == X[i, POINTS]) {
		# All trajectory points are inside the bounding box -> the trajectory is kept as is
	} else if (length(inside.bbox) > 0) {
		# Some trajectory points are outside the bounding box -> keeping only points inside the bounding box
		P <- P[inside.bbox]
		s <- apply(P, 1, function(x) paste(x, collapse = ","))
		s <- paste("[", paste("[", s, "]", sep = "", collapse = ","), "]", sep = "")
		set(X, i, "POINTS", nrow(P))
		set(X, i, "POLYLINE", s)
		set(X, i, "STATUS", "Truncated")
	} else {
		# All trajectory points are outside the bounding box -> the trajectory is deleted
		set(X, i, "POINTS", 0)
		set(X, i, "POLYLINE", "[]")
		set(X, i, "STATUS", "Deleted")
	}
	if (i %% 5000 == 0) {
		cat(paste0(i, "/", nrow(X)), "\n")
	}
}

cat("Trajectories unchanged:", X[STATUS == "Unchanged", .N], "\n")
cat("Trajectories truncated:", X[STATUS == "Truncated", .N], "\n")
empty <- X[POINTS == 0, which = TRUE]
if (length(empty) > 0) {
	X <- X[-empty]
}
cat("Empty trajectories removed:", length(empty), "\n")
cat("Number of remaining trajectories:", nrow(X), "\n")

X[, STATUS := NULL]

fwrite(X, "train.csv.2", quote = TRUE)

