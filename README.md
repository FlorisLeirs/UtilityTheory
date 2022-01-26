# UtilityTheory
A decision making system based on a score(utility) given to each possible action.

The behaviour of an AI can sometimes look very useless, like using a health potion while the AI has barely lost any health. In order to fix this problem the AI needs to know if an action is useful, this can be done by giving the actions utility(desirability of an action).
Using utility theory, behaviour of the AI is determined by assigning a utility to every possible behaviour. The decision made by the AI is based on information about its surroundings and its "needs"(giving the AI some context) e.g. enemies that are close, hunger...

Using the necessary information for each behaviour, a utility is calculated. If all utilities of all the possible actions for the AI are calculated, it will pick the behaviour with the highest utility, thus the most desirable behaviour.

## Calculating Utility
There are multiple ways to calculate the utility of your behaviours. For some behaviours calculations won't even be required. For example in my implementation I have a wander behavaiour, this just has a default utility of 0.05. Because of this it will only start if there are no other possible behaviours or the other behaviours are almost useless with the given context. This is ofcourse only one behaviour or possibly two or three that don't need utility. For all other behaviours calculations will be required. The results of these calculations all need to be in the same range of values. I used [0 - 1], but the range doesn't really matter as long as **all values are in the same range**. This means we really have to think about what variables are important for each behaviour in order to create a function that will give you the results you want, in the range you are using.
### Linear
One way of calculating utility is by using a linear function.

![linear equation](http://www.sciweavers.org/tex2img.php?eq=U%20%3D%20%20%5Cfrac%7Bvalue%7D%7BmaxValue%7D%20&bc=White&fc=Black&im=jpg&fs=12&ff=arev&edit=0)

<img src="https://user-images.githubusercontent.com/49953730/151132309-42154d63-af72-4b6d-89b4-e22775eb2547.png" width ="400" height="400"/>

This is simply a normalization of the value. This is the most straight-forward, but is not always desired becausee it just rises or falls at the same rate everywhere.

### Quadratic
With a quadratic equation the increase of utility rises along with your variable.

![quadratic equation](http://www.sciweavers.org/tex2img.php?eq=U%20%3D%20%20%28%5Cfrac%7Bvalue%7D%7BmaxValue%7D%29%20%20%5E%7Bexp%7D%20&bc=White&fc=Black&im=jpg&fs=12&ff=arev&edit=0)

<img src="https://user-images.githubusercontent.com/49953730/151134127-2ab3013b-e9ec-4c27-8461-efe691d4c38e.png" width ="400" height="400"/>

In the graph above 2 was used as exp.

You can increase your exponent to get a curve that stays flat longer and has a larger rise at the end. With an exponent in range ]0,1[ your graph will be the inverse. So a big rise for a low x value and a flat curve as x gets bigger.

### Logistic
A logistic function starts flat, has a sudden rise in the middle and is flat again at the end.

![logistic equation](http://www.sciweavers.org/tex2img.php?eq=U%20%3D%20%20%5Cfrac%7B1%7D%7B%201%20%2B%20e%5E%7B-x%7D%7D&bc=White&fc=Black&im=jpg&fs=12&ff=arev&edit=0)
 
 e = euler(2.718281828459045235360287...)
<img src="https://user-images.githubusercontent.com/49953730/151136051-36a6b759-4082-40f7-ab46-2c3df1c360dc.png" width ="800" height="400"/>

The result of a logistic equation can easily be clamped in a range of [-12, 12] (example) like the graph above. This makes it easy to normalize it to the correct range for utility values. Multiplying x will make the rise faster, dividing it will make the graph smoother. Adding a value to x will move the curve to the right(dividing does the opposite). The max y-value can be changed by adding or substrcting a value in the denominator. You can add or subtract values from the fraction to change the min y-value.

### Examples

To calculate when to flee I divide the health an enemy can afflict by the health the ai has left. This results in the Graph below.

![Flee equation](http://www.sciweavers.org/tex2img.php?eq=U%20%3D%20%20%5Cfrac%7BdamageEnemy%7D%7BcurrentHealth%7D&bc=White&fc=Black&im=jpg&fs=12&ff=arev&edit=0)

<img src="https://user-images.githubusercontent.com/49953730/151139605-d5f3f562-8a5e-4ea9-abd7-a08cdee356ab.png" width ="400" height="400"/>

To calculate when to shoot I use the following equation.

![Shoot equation](http://www.sciweavers.org/tex2img.php?eq=U%20%3D%20%20%28%5Cfrac%7BcurrentHealth%20-%20maxHealth%7D%7B%20-%20maxHealth%7D%29%5E%7B0.1%7D&bc=White&fc=Black&im=jpg&fs=12&ff=arev&edit=0)

<img src="https://user-images.githubusercontent.com/49953730/151143142-a101ea36-f81c-4c64-8bcb-fc7004012e99.png" width ="400" height="400"/>

To go to a house I use aa logistic equation where x is the amount of empty slots in my inventory.

![GoToHouseEquation](http://www.sciweavers.org/tex2img.php?eq=U%20%3D%20%5Cfrac%7B%5Cfrac%7B1%7D%7B%201%20%2B%20%28e%20%2A%205%29%5E%7B%28-x%20%2B%201%29%7D%7D%7D%7B1.3%7D&bc=White&fc=Black&im=jpg&fs=12&ff=arev&edit=0)

<img src="https://user-images.githubusercontent.com/49953730/151143881-df9e79c1-6dd5-4b62-ab73-8ff68a17b1c8.png" width ="200" height="400"/>


## Deciding which behaviour to use
There are multiple ways to decide which behaviour to use once the utility is calculated. The simplest and most straight-forward way is simply choosing the behaviour with the highest utility. This is what I used because good enough for my implementation. However can sometimes still seem really predictable and robotic. 

If you need a more "natural" ai it's best to go with another way of deciding. Something that will make it more natural is taking a handful of behaviours with the highest utilities and then randomly pick one of those. This way the ai will seem more natural. They still don't always do the most useful thing, but neither do humans, sometimes they will even do something completely useless. Overall the ai will still do very good.

As an example let's say the ai has very low health. It has multiple ways of healing, but the ai takes a random behaviour out of the top 5 and the ai chooses attacking the boss that can kill the ai with one more hit. This stupid decision made your ai die. This problem can be solved. To filter out the useless behaviour that can sometimes occur with choosing randomly there is a third technique. For the third technique you catagorize all your behaviours in more general behaviours. So now we would have a catagory heal with casting a healing spell, using a bandage and retreating to regenerate some health. Attack would also be a catagory with all the ways you can attack inside. Instead of looking at the behaviours, we start by giving every catagory a utility. Then the ai can pick the **catagory** with the highest utility, now you can do one of the above techniques inside the catagory. If there are no legal behaviours you simply take the second highest scoring catagory. Now the ai will pick the healing catagory and heal. Which way it will heal might still not always be the best if you randomly choose inside the catagory, but the ai will definitly do something useful.

## Oscillating behaviour
If two behaviours have about the same utility you will risk oscillating between behaviour. This means the ai will start rapidly switching between the behaviours which will probably cause it to get nothing done. To fix this problem there need to be limitations to when the behaviour can change. There are multiple ways to do this. The way I did in my implementation is checking if the behaviour can stop every frame. This means every behaviour has an individual way to tell the ai it can move on to another behaviour. For some behaviours this is a simple timer, my default behaviour always allows the ai to switch behaviour, sometimes a simple timer can also do the job just fine. For some of the behaviour there need to be some additional requirements in order to move on. Another way to keep the behaviours from oscillating is only changing the function which switches behaviours at certain intervals or if certain requirements are true. You can also let the behaviours themself call this function if the behaviour is finished.
