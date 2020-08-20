# Table of Contents

- [About the project](#chktropy)
	- [Build and install](#build-and-install)
	- [Usage](#usage)
	- [Input](#input)
- [Theory behind chktropy](#theory-behind-chktropy)
	- [Shannon's entropy](#shannons-entropy)
	- [Floating point considerations](#floating-point-considerations)
		- [Floating point errors](#floating-point-errors)
		- [Overflow](#overflow-error)
		- [Approximations](#approximations)
- [License](#license)
- [Contact](#contact)

# chktropy

Check the entropy of a password

chktropy reads a password on the standard input, calculates its Shannon's entropy, and displays it on the standard output.   
It can also display the number of characters in the password, the number of unique characters, and the number of passwords that can be generated with those characters.

# Build and Install

Build the project:

```
make
```

Install chktropy:

```
sudo make install
```

By default, chktropy is installed in `/usr/local/bin`

Uninstall:

```
sudo make uninstall
```

A manual is also installed both in english and french in `/usr/local/share/man`. It can be consulted with the following command:

```
man chktropy
```

## Usage


chktropy was designed for a quick usage in a data pipe.

The classical usage of chktropy is the following:

```
$ makepasswd -m 75 | chktropy -a
Input:              B0hB=(NGIj^@N~nP_VocLF4TdPnZcT*6V27UxW2H
Characters:         40
Unique characters:  32
Possible passwords: 1606938044258990275541962092341162602522202993782792835301376
Entropy:            138.629436
```
We can also write the rounded entropy to a file

```
$ pwgen -s1 30 | chktropy -er > file.txt
$ ls
file.txt
$ cat file.txt
98
```

It is also possible to give a password as an argument:

```
$ chktropy -s mypassword
Input:              mypassword
Entropy:            21.972246
```

This is very handy while generating passwords to check their strength.

To obtain help, you can type:

```
chktropy --help
```

## Input

Since real-world passwords use only 7 bits ASCII, chktropy only accepts  7 bits ASCII strings as an input.   
Otherwise the program stops and an error message is displayed.

No other check is done on the given string, since the set of characters accepted varies from one website to another.

Some only accept digits, others allow no special characters, and others have very few restrictions.    
It is up to you to make sure the password is accepted by the website you wish to use it on. It is not the job of chktropy.

# Theory behind chktropy

## Shannon's Entropy

Shannon's entropy roughly corresponds to the quantity of information contained in a data.

This is how it is calculated:
```
	Entropy = log2(nb_unique_characters ^ nb_characters)
```

Entropy is tightly linked to the quality of passwords.   
The higher the entropy, the more impredictable the data is, the stronger the passwords are.

## Floating point considerations

chktropy manipulates floating point numbers.   
Multiple checks and  roundings (some optional) are applied, and Shannon's entropy is calculated using the following function:

```
static double calculate_entropy(
	unsigned long int nb_unique_chars,
	unsigned long int nb_chars,
	long double *nb_passwords)
{
	*nb_passwords = powl((double)nb_unique_chars, (double)nb_chars);
	return logl(*nb_passwords);
}
```

Floating point numbers are subject to some unique behaviors:

	- specific errors (overflow, divide by zero)
	- relative approximations

Here is how chktropy takes care of those behaviors:

### Floating point errors

There are 4 different errors we should watch out for while doing calculations on floating point values:

	- divide by zero
	- overflow: the number is larger than the largest possible number of the data type
	- underflow: the number is smaller than the smallest possible number of the data type
	- invalid: no result for an operation (inf - inf for exemple)

For more informations, check `man 7 math_error`

chktropy is not concerned with underflow of divide by zero errors.   
The only ones we should actually care about are overflow and invalid, which itself results trom an overflow.

### Overflow error

While calculating Shannon's entropy in the above code, there is the risk that nb_passwords overflows the maximum possible value for a long double.     
If this were to happen, then the entropy would be infinite.

All math.h functions accept inf as a valid value, and it propagates through all functions as one would expect (see https://www.gnu.org/software/libc/manual/html_node/Infinity-and-NaN.html), so it should not be a problem.

However, an infinite entropy is only possible for an infinite password, so returning this value is just wrong.

### Approximations

Since floating points numbers use a finite amount of memory, they actually are approximations of the real value. The bigger the number, the bigger the approximation.
That means that the approximation error is relative to the number.

This is problematic for some calculations, since it implies that `a + (b + c)` might be different from `c + (a + b)`.

The maximum value this relative error can take is called the [machine_epsilon](https://en.wikipedia.org/wiki/Machine_epsilon). 

It is possible to calculate the specific epsilon to add to the current number to 'change' its value.   
We can think of it as a spacing between the current number and the next, bigger one.

As already said, this "spacing" is relative to the size of the number, so the bigger the number gets, the less accurate it becomes. Thus it is acceptable to give an arbitrary high value to the number of possible passwords (and thus to entropy) for really big numbers since this value would be valid for quite a big range of different numbers.

If the number of passwords that can be generated is higher than LDBL_MAX, then chktropy considers that the maximum number of passwords is LDBL_MAX.

## Why is it acceptable

As indicated by `man 2 float.h`, LDBL_MAX is defined as at least 1E+37 on a x86_64 processor, which is huge.   
For exemple, such a password is stronger than a 2600 characters password with 78 unique characters.   
That is 11327 characters of entropy.   
For reference, a human generated password has an average entropy of ~33 characters.

It is very unlikely that anyone would use such a big password.   
It is even more unlikely that any service would allow anyone to use such a password.   
For this reasons, chktropy does not use arbitrary precision floating point numbers, even though it has been considered to use gnu gmp library.

# License

Distributed under GPL License. See `COPYING` for more information.

# Contact

Mathias Schmitt - mathiaspeterhorst@gmail.com     
Project link: https://github.com/mphschmitt/chktropy