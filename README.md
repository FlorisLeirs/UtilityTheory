# UtilityTheory
A decision making system based on a score(utility) given to each possible action.

The behaviour of an AI can sometimes look very useless, like using a health potion while the AI has barely lost any health. In order to fix this problem the AI needs to know if an action is useful, this can be done by giving the actions utility(desirability of an action).
Using utility theory, behaviour of the AI is determined by assigning a utility to every possible behaviour. The decision made by the AI is based on information about its surroundings and its "needs"(giving the AI some context) e.g. enemies that are close, hunger...

Using the necessary information for each behaviour, a utility is calculated. If all utilities of all the possible actions for the AI are calculated, it will pick the behaviour with the highest utility, thus the most desirable behaviour.
