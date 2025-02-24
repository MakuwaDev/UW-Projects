# Stock Exchange Trading System

## Short task summary/translation:

The Stock Exchange Trading System facilitates stock trading for various companies. It allows investors to place buy or sell orders. Each order represents an investor's intent to buy or sell shares of a specific company and must include:

    Order Type: Buy / Sell
    Stock Identifier: A non-empty ASCII string (A-Z) with a maximum length of 5 characters
    Number of Shares: A positive integer
    Price Limit: A positive integer

Each order applies to a single company and includes a price limit (upper bound for buy orders, lower bound for sell orders). A transaction occurs when a buy order's price matches or exceeds a corresponding sell order's price.
### Order Matching Example

Consider the following order book for a particular stock:
| **Buy**         |                  |           | **Sell**        |           |                  |
|-----------------|------------------|-----------|-----------------|-----------|------------------|
| **Order no.**   | **Shares count** | **Price** | **Order no.**   | **Price** | **Shares count** |
| 4               | **100**          | **125**   | 1               | **123**   | **10**           |
|                 |  40              | 122       | 2               | **124**   | **25**           |
|                 |  10              | 121       | 3               | **125**   | **30**           |
|                 |  30              | 120       |                 | 126       | 20               |
|                 |  10              | 119       |                 | 127       | 60               |
|                 |  20              | 118       |                 |           |                  |

The highest buy order (Order 4, 100 shares at 125) will be partially executed in three transactions:

    Buy 10 shares from Order 1 at 123
    Buy 25 shares from Order 2 at 124
    Buy 30 shares from Order 3 at 125

A total of 65 shares will be acquired, leaving 35 shares pending execution at a limit price of 125.  
## Order Execution Rules

The system operates in turn-based mode. Order execution follows:

    Price Priority: Buy orders with higher limits match sell orders with lower limits.
    Time Priority: If multiple orders have the same price, the earliest one is executed first.
    Execution Price: The trade occurs at the price of the older order (earlier submission time).

For example, if an earlier sell order has a limit of 100 and a later buy order has a limit of 102, the transaction occurs at 100 (the older order’s price).
## Order Expiry Options

Orders can have different validity durations:

    Immediate Execution Order: Must be at least partially fulfilled in the same turn; unfulfilled portions are removed.
    Good-Till-Filled Order: Stays in the system until fully executed.
    Execute or Cancel Order: Must be executed fully within the same turn; otherwise, it is removed.
    Good-Till-Turn Order: Remains valid until a specified turn unless fulfilled earlier.

## Investor Behavior and Decision-Making

In each turn, investors can:

    Query the current turn number
    Retrieve the last transaction price of a stock (multiple times)
    Submit at most one buy/sell order for a selected stock

The system processes all investors' decisions randomly each turn.
### Investor Strategies

The system supports different investor strategies:

    RANDOM – Makes random buy/sell decisions (random stock, quantity, and price within limits).
    SMA (Simple Moving Average) – Uses technical analysis to decide based on:
        SMA(5): Average price over the last 5 turns
        SMA(10): Average price over the last 10 turns
        Buy Signal: SMA(5) crosses SMA(10) from below
        Sell Signal: SMA(5) crosses SMA(10) from above
        Investors start using this method only after turn 10.

The system should allow adding new investor types easily.
## Trading Constraints

    Stock prices and order price limits are positive integers.
    Order limits must not differ by more than ±10 from the last transaction price.
    Investors must have enough assets to place an order (stocks for selling, cash for buying).
    Short selling is not allowed – investors cannot sell stocks they do not own.
    Orders that fail due to insufficient funds or shares are canceled.

## Program Input and Execution

The program reads:

    Input file (initial state)
    Number of simulation turns

Example execution:

java GPWSimulation input.txt 100000

### Input File Format

\# 6 investors: 4 RANDOM, 2 SMA
R R R R S S

\# Stocks and last transaction prices
APL:145 MSFT:300 GOOGL:2700

\# Initial portfolio (same for all investors)
100000 APL:5 MSFT:15 GOOGL:3

    Lines starting with # are ignored
    Investors are represented by R (RANDOM) and S (SMA)
    Stock symbols and prices are given as TICKER:PRICE
    All investors start with the same cash balance and stock holdings

## Output Format

After simulation, the final portfolio of each investor is printed:

100000 APL:5 MSFT:15 GOOGL:3
100000 APL:5 MSFT:15 GOOGL:3
100000 APL:5 MSFT:15 GOOGL:3
100000 APL:5 MSFT:15 GOOGL:3
100000 APL:5 MSFT:15 GOOGL:3
100000 APL:5 MSFT:15 GOOGL:3

## Additional Features

    The system should log all trading operations for debugging.
    The total sum of cash and stocks remains constant throughout the simulation.
    Input validation must check if stocks in investor portfolios exist in the trading system.

## JUnit Testing Requirements

    Verify order execution logic
    Test different investor types and configurations
    Check handling of edge cases (e.g., invalid input, insufficient funds)
