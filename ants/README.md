Simple simulation of evolution. Ants eat candy to survive and reproduce. Candies have a limited supply, so ants compete for them. When reproducing, ants "mutate" slightly; they can become a bit faster or slower.

I have run several "experiments" with this code, changing some simple rules. For example:

1. Simple rule: An ant has as many kids as candies it has eaten. Obviously, in this scenario, ants get faster and faster.

2. A somewhat interesting one: similar to the first experiment but punishes ants for being too fast; the faster an ant is, the more food it needs to reproduce. In my run fast ants ate a lot but were not able to reproduce enough to sustain the population. After a few rounds only a couple of slow ants survived and thrived having all the food for themselves. Then they populated whole area and began to get faster with every generation.

Tried to look for some interesting behavior by changing other parameters like spawning randomly vs in groups around where the parent were, having all ants start with same stats or random stats, changing some constants, etc but I am yet to find it.
