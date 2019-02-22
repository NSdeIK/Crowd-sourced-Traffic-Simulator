Y <- read.csv("pkdd15-subset.csv", colClasses = c(rep("numeric", 9), "character"), nrows = -1)

YY <- Y[0,]
remaining = 0
for (i in 1:nrow(Y)) {
	tmp <- unlist(strsplit(Y[i, "polyline"], "]", fixed = TRUE))
	if (! identical(tmp, "[")) {
		tmp <- sapply(tmp[1:(length(tmp) - 1)], function(z) substr(z, 3, nchar(z)), USE.NAMES = FALSE)
		P <- do.call(rbind, lapply(strsplit(tmp, ",", fixed = TRUE), as.numeric))
		P <- subset(P, -8.6518 <= P[,1] & P[,1] <= -8.5771 & 41.1129 <= P[,2] & P[,2] <= 41.1756)
		if (nrow(P) > 0) {
			s <- apply(P, 1, function(x) paste(x, collapse=","))
			s <- paste("[", paste("[", s, "]", sep="", collapse=","), "]", sep="")
			r <- Y[i,]
			r$points <- nrow(P)
			r$polyline <- s
			remaining <- remaining + 1
			YY <- rbind(YY, r)
		} else {
#			cat("Dropping trajectory", i, "\n")
		}
	}
}
cat("Remaining trajectories:", remaining, "\n")
write.csv(YY, file = "pkdd15-subset-bbox-all.csv", row.names = FALSE)
s <- sort(sample(1:nrow(YY), nrow(YY) * 0.6))
write.csv(YY[-s,], file = "pkdd15-subset-bbox-test.csv", row.names = FALSE)
