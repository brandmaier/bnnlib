#' Some surprising facts about bananas
#'
#' @export


banana.facts <- function()
{
  all.facts <- list("Bananas can be found in other colors, too, including red.",
                    "Banana plants are not trees",
                    "The scientific name for banana is musa sapientum, which means >fruit of the wise men.<",
                    "Bananas float in water. Go try it!",
                    "Bananas are berries",
                    "Humans share about 50% of our DNA with bananas.",
                    "A banana contains naturally occurring radioactive material in the form of potassium-40.",
                    "Banana equivalent dose (BED) is an informal measurement of ionizing radiation exposure"
                    )
  cat(crayon::bgYellow("Banana fact:"),sample(all.facts,size=1)[[1]])
}