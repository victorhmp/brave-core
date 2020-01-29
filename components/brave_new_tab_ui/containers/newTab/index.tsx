/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import { DragDropContext } from 'react-dnd'
import HTML5Backend from 'react-dnd-html5-backend'

// Components
import Stats from './stats'
import Block from './block'
import FooterInfo from './footerInfo'
import SiteRemovalNotification from './notification'
import {
  ClockWidget as Clock,
  ListWidget as List,
  RewardsWidget as Rewards,
  BinanceWidget as Binance
} from '../../components/default'
import * as Page from '../../components/default/page'
import BrandedWallpaperLogo from '../../components/default/brandedWallpaper/logo'
import VisibilityTimer from '../../helpers/visibilityTimer'

interface Props {
  newTabData: NewTab.State
  actions: any
  saveShowBackgroundImage: (value: boolean) => void
  saveShowClock: (value: boolean) => void
  saveShowTopSites: (value: boolean) => void
  saveShowStats: (value: boolean) => void
  saveShowRewards: (value: boolean) => void
  saveBrandedWallpaperOptIn: (value: boolean) => void
  saveShowBinance: (value: boolean) => void
}

interface State {
  onlyAnonWallet: boolean
  showSettingsMenu: boolean
  backgroundHasLoaded: boolean
}

function GetBackgroundImageSrc (props: Props) {
  if (!props.newTabData.showBackgroundImage) {
    return undefined
  }
  if (props.newTabData.brandedWallpaperData) {
    const wallpaperData = props.newTabData.brandedWallpaperData
    if (wallpaperData && wallpaperData.wallpaperImageUrl) {
      return wallpaperData.wallpaperImageUrl
    }
  }
  if (props.newTabData.backgroundImage && props.newTabData.backgroundImage.source) {
    return props.newTabData.backgroundImage.source
  }
  return undefined
}

function GetIsShowingBrandedWallpaper (props: Props) {
  const { newTabData } = props
  return newTabData.brandedWallpaperData ? true : false
}

function GetShouldShowBrandedWallpaperNotification (props: Props) {
  return GetIsShowingBrandedWallpaper(props) &&
  !props.newTabData.isBrandedWallpaperNotificationDismissed
}

class NewTabPage extends React.Component<Props, State> {
  state = {
    onlyAnonWallet: false,
    showSettingsMenu: false,
    backgroundHasLoaded: false
  }
  imageSource?: string = undefined
  timerIdForBrandedWallpaperNotification?: number = undefined
  onVisiblityTimerExpired = () => {
    this.dismissBrandedWallpaperNotification(false)
  }
  visibilityTimer = new VisibilityTimer(this.onVisiblityTimerExpired, 4000)

  componentDidMount () {
    // if a notification is open at component mounting time, close it
    this.props.actions.onHideSiteRemovalNotification()
    this.imageSource = GetBackgroundImageSrc(this.props)
    this.trackCachedImage()
    if (GetShouldShowBrandedWallpaperNotification(this.props)) {
      this.trackBrandedWallpaperNotificationAutoDismiss()
    }
  }

  componentDidUpdate (prevProps: Props) {
    const oldImageSource = GetBackgroundImageSrc(prevProps)
    const newImageSource = GetBackgroundImageSrc(this.props)
    this.imageSource = newImageSource
    if (newImageSource && oldImageSource !== newImageSource) {
      this.trackCachedImage()
    }
    if (oldImageSource &&
      !newImageSource) {
      // reset loaded state
      this.setState({ backgroundHasLoaded: false })
    }
    if (!GetShouldShowBrandedWallpaperNotification(prevProps) &&
        GetShouldShowBrandedWallpaperNotification(this.props)) {
      this.trackBrandedWallpaperNotificationAutoDismiss()
    }

    if (GetShouldShowBrandedWallpaperNotification(prevProps) &&
        !GetShouldShowBrandedWallpaperNotification(this.props)) {
      this.stopWaitingForBrandedWallpaperNotificationAutoDismiss()
    }

  }

  trackCachedImage () {
    if (this.imageSource) {
      const imgCache = new Image()
      imgCache.src = this.imageSource
      console.timeStamp('image start loading...')
      imgCache.onload = () => {
        console.timeStamp('image loaded')
        this.setState({
          backgroundHasLoaded: true
        })
      }
    }
  }

  trackBrandedWallpaperNotificationAutoDismiss () {
    // Wait until page has been visible for an uninterupted Y seconds and then
    // dismiss the notification.
    this.visibilityTimer.startTracking()
  }

  stopWaitingForBrandedWallpaperNotificationAutoDismiss () {
    this.visibilityTimer.stopTracking()
  }

  onDraggedSite = (fromUrl: string, toUrl: string, dragRight: boolean) => {
    this.props.actions.siteDragged(fromUrl, toUrl, dragRight)
  }

  onDragEnd = (url: string, didDrop: boolean) => {
    this.props.actions.siteDragEnd(url, didDrop)
  }

  onToggleBookmark (site: NewTab.Site) {
    if (site.bookmarked === undefined) {
      this.props.actions.bookmarkAdded(site.url)
    } else {
      this.props.actions.bookmarkRemoved(site.url)
    }
  }

  onTogglePinnedTopSite (site: NewTab.Site) {
    if (!site.pinned) {
      this.props.actions.sitePinned(site.url)
    } else {
      this.props.actions.siteUnpinned(site.url)
    }
  }

  onIgnoredTopSite (site: NewTab.Site) {
    this.props.actions.siteIgnored(site.url)
  }

  toggleShowBackgroundImage = () => {
    this.props.saveShowBackgroundImage(
      !this.props.newTabData.showBackgroundImage
    )
  }

  toggleShowClock = () => {
    this.props.saveShowClock(
      !this.props.newTabData.showClock
    )
  }

  toggleShowStats = () => {
    this.props.saveShowStats(
      !this.props.newTabData.showStats
    )
  }

  toggleShowTopSites = () => {
    this.props.saveShowTopSites(
      !this.props.newTabData.showTopSites
    )
  }

  toggleShowRewards = () => {
    this.props.saveShowRewards(
      !this.props.newTabData.showRewards
    )
  }

  disableBrandedWallpaper = () => {
    this.props.saveBrandedWallpaperOptIn(false)
  }

  toggleShowBrandedWallpaper = () => {
    this.props.saveBrandedWallpaperOptIn(
      !this.props.newTabData.brandedWallpaperOptIn
    )
  }

  toggleShowBinance = () => {
    this.props.saveShowBinance(
      !this.props.newTabData.showBinance
    )
  }

  connectBinance = () => {
    this.props.actions.connectToBinance()
  }

  onApiKeysInvalid = () => {
    this.props.actions.onApiKeysInvalid()
  }

  setApiKeys = (apiKey: string, apiSecret: string) => {
    this.props.actions.setApiKeys(apiKey, apiSecret)
    setTimeout(() => {
      chrome.binance.validateAPIKey((status: number, unauthorized: boolean) => {
        if (unauthorized || (status < 200 || status > 299)) {
          this.props.actions.onApiCredsError()
        } else {
          this.props.actions.onValidApiCreds()
        }
      })
    }, 1000)
  }

  setBinanceBalances = (balances: Record<string, string>) => {
    this.props.actions.onBinanceBalances(balances)
  }

  setBTCUSDPrice = (price: string) => {
    this.props.actions.onBTCUSDPrice(price)
  }

  setAssetBTCPrice = (ticker: string, price: string) => {
    this.props.actions.onAssetBTCPrice(ticker, price)
  }

  onBinanceUserTLD = (userTLD: NewTab.BinanceTLD) => {
    this.props.actions.onBinanceUserTLD(userTLD)
  }

  openBinanceUrl = (route: string) => {
    let path = ''
    const { userTLD } = this.props.newTabData.binanceState

    switch (route) {
      case 'deposit':
        path = '/usercenter/wallet/deposit/BTC'
        break
      case 'trade':
        path = '/usercenter/wallet/balances'
        break
      case 'details':
        path = '/usercenter/dashboard/overview'
        break
      case 'newKey':
        path = '/usercenter/settings/api-management '
        break
    }

    window.open(`https://www.binance.${userTLD}/en${path}`, '_blank')
  }

  depositBinance = () => {
    this.openBinanceUrl('deposit')
  }

  tradeBinance = () => {
    this.openBinanceUrl('trade')
  }

  binanceDetails = () => {
    this.openBinanceUrl('details')
  }

  generateNewKey = () => {
    this.openBinanceUrl('newKey')
  }

  setHideBalance = (hide: boolean) => {
    this.props.actions.setHideBalance(hide)
  }

  disconnectBinance = () => {
    this.props.actions.disconnectBinance()
  }

  enableAds = () => {
    chrome.braveRewards.saveAdsSetting('adsEnabled', 'true')
  }

  enableRewards = () => {
    chrome.braveRewards.saveSetting('enabledMain', '1')
  }

  createWallet = () => {
    this.props.actions.createWallet()
  }

  dismissBrandedWallpaperNotification = (isUserAction: boolean) => {
    this.props.actions.dismissBrandedWallpaperNotification(isUserAction)
  }

  dismissNotification = (id: string) => {
    this.props.actions.dismissNotification(id)
  }

  closeSettings = () => {
    this.setState({ showSettingsMenu: false })
  }

  toggleSettings = () => {
    this.setState({ showSettingsMenu: !this.state.showSettingsMenu })
  }

  renderBinanceContent () {
    const { newTabData } = this.props

    if (!newTabData.showBinance) {
      return null
    }

    return (
      <Page.GridItemBinance>
        <Binance
          {...newTabData.binanceState}
          menuPosition={'left'}
          hideWidget={this.toggleShowBinance}
          connectBinance={this.connectBinance}
          onBinanceDetails={this.binanceDetails}
          onBinanceDeposit={this.depositBinance}
          onBinanceTrade={this.tradeBinance}
          onSetHideBalance={this.setHideBalance}
          onGenerateNewKey={this.generateNewKey}
          onBinanceBalances={this.setBinanceBalances}
          onBinanceUserTLD={this.onBinanceUserTLD}
          onBTCUSDPrice={this.setBTCUSDPrice}
          onSetApiKeys={this.setApiKeys}
          onApiKeysInvalid={this.onApiKeysInvalid}
          onAssetBTCPrice={this.setAssetBTCPrice}
          onDisconnectBinance={this.disconnectBinance}
          textDirection={newTabData.textDirection}
        />
      </Page.GridItemBinance>
    )
  }

  renderRewardsContent () {
    const { newTabData } = this.props
    const {
      rewardsState,
      showRewards: rewardsWidgetOn
    } = newTabData
    const isShowingBrandedWallpaper = GetIsShowingBrandedWallpaper(this.props)
    const shouldShowBrandedWallpaperNotification = GetShouldShowBrandedWallpaperNotification(this.props)
    const shouldShowRewardsWidget = rewardsWidgetOn || shouldShowBrandedWallpaperNotification
    return shouldShowRewardsWidget && (
      <Page.GridItemRewards>
        <Rewards
          {...rewardsState}
          preventFocus={!rewardsWidgetOn}
          onCreateWallet={this.createWallet}
          onEnableAds={this.enableAds}
          onEnableRewards={this.enableRewards}
          isShowingBrandedWallpaper={isShowingBrandedWallpaper}
          showBrandedWallpaperNotification={shouldShowBrandedWallpaperNotification}
          onDisableBrandedWallpaper={this.disableBrandedWallpaper}
          brandedWallpaperData={newTabData.brandedWallpaperData}
          textDirection={newTabData.textDirection}
          hideWidget={this.toggleShowRewards}
          isNotification={!rewardsWidgetOn}
          onDismissNotification={this.dismissNotification}
          onDismissBrandedWallpaperNotification={this.dismissBrandedWallpaperNotification}
          menuPosition={'left'}
        />
      </Page.GridItemRewards>
    )
  }

  render () {
    const { newTabData, actions } = this.props
    const { showSettingsMenu } = this.state

    if (!newTabData) {
      return null
    }

    const hasImage = this.imageSource !== undefined
    const isShowingBrandedWallpaper = newTabData.brandedWallpaperData ? true : false
    const showTopSites = !!this.props.newTabData.gridSites.length && newTabData.showTopSites
    const rewardsContent = this.renderRewardsContent()
    const binanceContent = this.renderBinanceContent()

    return (
      <Page.App dataIsReady={newTabData.initialDataLoaded}>
        <Page.PosterBackground
          hasImage={hasImage}
          imageHasLoaded={this.state.backgroundHasLoaded}
        >
          {hasImage &&
            <img src={this.imageSource} />
          }
        </Page.PosterBackground>
        {hasImage &&
          <Page.Gradient
            imageHasLoaded={this.state.backgroundHasLoaded}
          />
        }
        <Page.Page
            showClock={newTabData.showClock}
            showStats={newTabData.showStats}
            showRewards={!!rewardsContent}
            showTopSites={showTopSites}
            showBinance={!!binanceContent}
            showBrandedWallpaper={isShowingBrandedWallpaper}
        >
          {newTabData.showStats &&
          <Page.GridItemStats>
            <Stats
              textDirection={newTabData.textDirection}
              stats={newTabData.stats}
              hideWidget={this.toggleShowStats}
              menuPosition={'right'}
            />
          </Page.GridItemStats>
          }
          {newTabData.showClock &&
          <Page.GridItemClock>
            <Clock
              textDirection={newTabData.textDirection}
              hideWidget={this.toggleShowClock}
              menuPosition={'left'}
            />
          </Page.GridItemClock>
          }
          {showTopSites &&
          <Page.GridItemTopSites><List
            blockNumber={this.props.newTabData.gridSites.length}
            textDirection={newTabData.textDirection}
            menuPosition={'right'}
            hideWidget={this.toggleShowTopSites}
          >
            {
              this.props.newTabData.gridSites.map((site: NewTab.Site) =>
                <Block
                  key={site.url}
                  id={site.url}
                  title={site.title}
                  href={site.url}
                  favicon={site.favicon}
                  style={{ backgroundColor: site.themeColor || site.computedThemeColor }}
                  onToggleBookmark={this.onToggleBookmark.bind(this, site)}
                  onPinnedTopSite={this.onTogglePinnedTopSite.bind(this, site)}
                  onIgnoredTopSite={this.onIgnoredTopSite.bind(this, site)}
                  onDraggedSite={this.onDraggedSite}
                  onDragEnd={this.onDragEnd}
                  isPinned={site.pinned}
                  isBookmarked={site.bookmarked !== undefined}
                />
              )
            }
          </List></Page.GridItemTopSites>
          }
          {
            this.props.newTabData.showSiteRemovalNotification
            ? <Page.GridItemNotification>
                <SiteRemovalNotification actions={actions} />
              </Page.GridItemNotification>
            : null
          }
            {rewardsContent}
            {binanceContent}
          <Page.Footer>
            <Page.FooterContent>
            {isShowingBrandedWallpaper && newTabData.brandedWallpaperData &&
             newTabData.brandedWallpaperData.logo &&
             <Page.GridItemBrandedLogo>
              <BrandedWallpaperLogo
                menuPosition={'right'}
                textDirection={newTabData.textDirection}
                data={newTabData.brandedWallpaperData.logo}
              />
            </Page.GridItemBrandedLogo>}
            <FooterInfo
              textDirection={newTabData.textDirection}
              onClickOutside={this.closeSettings}
              backgroundImageInfo={newTabData.backgroundImage}
              onClickSettings={this.toggleSettings}
              showSettingsMenu={showSettingsMenu}
              showPhotoInfo={!isShowingBrandedWallpaper && newTabData.showBackgroundImage}
              toggleShowBackgroundImage={this.toggleShowBackgroundImage}
              toggleShowClock={this.toggleShowClock}
              toggleShowStats={this.toggleShowStats}
              toggleShowTopSites={this.toggleShowTopSites}
              toggleBrandedWallpaperOptIn={this.toggleShowBrandedWallpaper}
              showBackgroundImage={newTabData.showBackgroundImage}
              showClock={newTabData.showClock}
              showStats={newTabData.showStats}
              showTopSites={newTabData.showTopSites}
              showRewards={newTabData.showRewards}
              brandedWallpaperOptIn={newTabData.brandedWallpaperOptIn}
              allowBrandedWallpaperUI={newTabData.featureFlagBraveNTPBrandedWallpaper}
              toggleShowRewards={this.toggleShowRewards}
              showBinance={newTabData.showBinance}
              toggleShowBinance={this.toggleShowBinance}
            />
            </Page.FooterContent>
          </Page.Footer>
        </Page.Page>
      </Page.App>
    )
  }
}

export default DragDropContext(HTML5Backend)(NewTabPage)
