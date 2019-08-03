library(ggplot2)
library(magrittr)
library(osmdata)
library(osmplotr)
library(sf)
library(stringr)
library(tools)

file <- "corrected_DIST_50k_1min.txt"
highway_color <- "red"
min_highway_width <- 0.1
max_highway_width <- 3
lower_bound <- 10

processLine <- function(line) {
	str_match(line, "^\\[(.*)\\]$")[1, 2] %>% str_split(",", simplify = TRUE) %>% str_trim() %>% str_split("=", simplify = TRUE) -> M
	Y <- data.frame(name = I(M[,1]), count = as.integer(M[,2]))
}

bbox <- get_bbox(c(-8.6518, 41.1756, -8.5771, 41.1129))
highways <- extract_osm_objects(key = "highway", bbox = bbox)

lines <- readLines(file)

sapply(lines, function(line) processLine(line)$count) %>% unlist() -> counts
names(counts) <- NULL
qplot(counts, geom = "histogram", binwidth = 10)

for (i in 1:length(lines)) {
	cat("line_number =", i, "\n")
	highways_to_plot <- processLine(lines[i])

	highways_to_plot <- subset(highways_to_plot, subset = count >= lower_bound)

	highways_to_plot_named <- subset(highways_to_plot, subset = ! str_detect(name, "^UNS"))
	highways_to_plot_unnamed <- subset(highways_to_plot, subset = str_detect(name, "^UNS"))

	highways_selected_named <- subset(highways, subset = name %in% highways_to_plot_named$name)
	highways_selected_unnamed <- subset(highways, subset = osm_id %in% str_sub(highways_to_plot_unnamed$name, 5))

	map <- osm_basemap(bbox = bbox, bg = "ghostwhite")
	map <- add_osm_objects(map, highways, col = "lightgray", size = 0.1)

	if (! plyr::empty(highways_selected_named)) {
		for (cnt in unique(sort(highways_to_plot_named$count))) {
			cat("count =", cnt, "\n")
			highways_with_count <- subset(highways_to_plot_named, subset = count == cnt)$name
			tmp <- subset(highways_selected_named, subset = name %in% highways_with_count)
			if (! plyr::empty(tmp)) {
				map <- add_osm_objects(map, tmp, col = highway_color, size = min_highway_width + (cnt * (max_highway_width - min_highway_width) / diff(range(counts))))
			}
		}
	}

	if (! plyr::empty(highways_selected_unnamed)) {
		for (cnt in unique(sort(highways_to_plot_unnamed$count))) {
			cat("count =", cnt, "\n")
			highways_with_count <- subset(highways_to_plot_unnamed, subset = count == cnt)$name
			tmp <- subset(highways_selected_unnamed, subset = name %in% highways_with_count)
			if (! plyr::empty(tmp)) {
				map <- add_osm_objects(map, tmp, col = highway_color, size = min_highway_width + (cnt * (max_highway_width - min_highway_width) / diff(range(counts))))
			}
		}
	}

	print_osm_map(map, file = paste0(file_path_sans_ext(file), "_", i, ".png"))
}

